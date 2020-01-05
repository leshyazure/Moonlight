/*
 *
 *  	Created on: 1 jan 2020
 *      Author: Leshy Azure
 *      www.azurescens.eu
 *
 */

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <moonlight.h>
#include "esp_http_server.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "cJSON.h"
#include "driver/ledc.h"

static const char *REST_TAG = "server";
#define REST_CHECK(a, str, goto_tag, ...)                                                  \
		do                                                                                 \
		{                                                                                  \
			if (!(a))                                                                      \
			{                                                                              \
				ESP_LOGE(REST_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
				goto goto_tag;                                                             \
			}                                                                              \
		} while (0)

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)

typedef struct rest_server_context {
	char base_path[ESP_VFS_PATH_MAX + 1];
	char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

/* Set HTTP response content type according to file extension */
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
	const char *type = "text/plain";
	if (CHECK_FILE_EXTENSION(filepath, ".html")) {
		type = "text/html";
	} else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
		type = "application/javascript";
	} else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
		type = "text/css";
	} else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
		type = "image/png";
	} else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
		type = "image/x-icon";
	} else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
		type = "text/xml";
	}
	return httpd_resp_set_type(req, type);
}

/* Send HTTP response with the contents of the requested file */
static esp_err_t rest_common_get_handler(httpd_req_t *req)
{
	char filepath[FILE_PATH_MAX];

	rest_server_context_t *rest_context = (rest_server_context_t *)req->user_ctx;
	strlcpy(filepath, rest_context->base_path, sizeof(filepath));
	if (req->uri[strlen(req->uri) - 1] == '/') {
		strlcat(filepath, "/index.html", sizeof(filepath));
	} else {
		strlcat(filepath, req->uri, sizeof(filepath));
	}
	int fd = open(filepath, O_RDONLY, 0);
	if (fd == -1) {
		ESP_LOGE(REST_TAG, "Failed to open file : %s", filepath);
		/* Respond with 500 Internal Server Error */
		httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
		return ESP_FAIL;
	}

	set_content_type_from_file(req, filepath);

	char *chunk = rest_context->scratch;
	ssize_t read_bytes;
	do {
		/* Read file in chunks into the scratch buffer */
		read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
		if (read_bytes == -1) {
			ESP_LOGE(REST_TAG, "Failed to read file : %s", filepath);
		} else if (read_bytes > 0) {
			/* Send the buffer contents as HTTP response chunk */
			if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
				close(fd);
				ESP_LOGE(REST_TAG, "File sending failed!");
				/* Abort sending file */
				httpd_resp_sendstr_chunk(req, NULL);
				/* Respond with 500 Internal Server Error */
				httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
				return ESP_FAIL;
			}
		}
	} while (read_bytes > 0);
	/* Close file after sending complete */
	close(fd);
	ESP_LOGI(REST_TAG, "File sending complete");
	/* Respond with an empty chunk to signal HTTP response completion */
	httpd_resp_send_chunk(req, NULL, 0);
	return ESP_OK;
}

/* Handler for color control */

static esp_err_t setup_color_handler(httpd_req_t *req)
{
	int total_len = req->content_len;
	int cur_len = 0;
	char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
	int received = 0;
	if (total_len >= SCRATCH_BUFSIZE) {
		/* Respond with 500 Internal Server Error */
		httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
		return ESP_FAIL;
	}
	while (cur_len < total_len) {
		received = httpd_req_recv(req, buf + cur_len, total_len);
		if (received <= 0) {
			/* Respond with 500 Internal Server Error */
			httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
			return ESP_FAIL;
		}
		cur_len += received;
	}
	buf[total_len] = '\0';

	cJSON *root = cJSON_Parse(buf);
	int rOn = cJSON_GetObjectItem(root, "redOn")->valueint;
	int gOn = cJSON_GetObjectItem(root, "greenOn")->valueint;
	int bOn = cJSON_GetObjectItem(root, "blueOn")->valueint;
	int wOn = cJSON_GetObjectItem(root, "whiteOn")->valueint;
	int rOff = cJSON_GetObjectItem(root, "redOff")->valueint;
	int gOff = cJSON_GetObjectItem(root, "greenOff")->valueint;
	int bOff = cJSON_GetObjectItem(root, "blueOff")->valueint;
	int wOff = cJSON_GetObjectItem(root, "whiteOff")->valueint;

	//  int dur = cJSON_GetObjectItem(root, "duration")->valueint;
	//   int fin = cJSON_GetObjectItem(root, "fadein")->valueint;
	//  int fout = cJSON_GetObjectItem(root, "fadeout")->valueint;

	cJSON_Delete(root);
	httpd_resp_sendstr(req, "Post control values successfully");
	ESP_LOGI(REST_TAG, "Set PWM: Red = %d, Green = %d, Blue = %d, White = %d", rOn, gOn, bOn, wOn);
	setLEDOn(rOn, gOn, bOn, wOn);
	setLEDOff(rOff, gOff, bOff, wOff);
	previewColor();

	return ESP_OK;
}

/* Handler for timing control */

static esp_err_t set_timing_handler(httpd_req_t *req)
{
	int total_len = req->content_len;
	int cur_len = 0;
	char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
	int received = 0;
	if (total_len >= SCRATCH_BUFSIZE) {
		/* Respond with 500 Internal Server Error */
		httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
		return ESP_FAIL;
	}
	while (cur_len < total_len) {
		received = httpd_req_recv(req, buf + cur_len, total_len);
		if (received <= 0) {
			/* Respond with 500 Internal Server Error */
			httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
			return ESP_FAIL;
		}
		cur_len += received;
	}
	buf[total_len] = '\0';
	cJSON *root = cJSON_Parse(buf);
	int dur = cJSON_GetObjectItem(root, "duration")->valueint;
	int fin = cJSON_GetObjectItem(root, "fadein")->valueint;
	int fout = cJSON_GetObjectItem(root, "fadeout")->valueint;
	cJSON_Delete(root);
	httpd_resp_sendstr(req, "Post control values successfully");
	ESP_LOGI(REST_TAG, "Set Timing: Fade in = %d, Fade out = %d, Duration = %d", fin, fout, dur);
	setTiming(fin, fout, dur);
	return ESP_OK;

}

/* Handler for threshold control */

static esp_err_t set_threshold_handler(httpd_req_t *req)
{
	int total_len = req->content_len;
	int cur_len = 0;
	char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
	int received = 0;
	if (total_len >= SCRATCH_BUFSIZE) {
		/* Respond with 500 Internal Server Error */
		httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
		return ESP_FAIL;
	}
	while (cur_len < total_len) {
		received = httpd_req_recv(req, buf + cur_len, total_len);
		if (received <= 0) {
			/* Respond with 500 Internal Server Error */
			httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
			return ESP_FAIL;
		}
		cur_len += received;
	}
	buf[total_len] = '\0';
	cJSON *root = cJSON_Parse(buf);
	int thr = cJSON_GetObjectItem(root, "threshold")->valueint;
	cJSON_Delete(root);
	httpd_resp_sendstr(req, "Post control values successfully");
	ESP_LOGI(REST_TAG, "Set threshold to %d", thr);
	setThreshold(thr);
	return ESP_OK;

}

/* Handler for preview control */

static esp_err_t set_preview_handler(httpd_req_t *req)
{
	int total_len = req->content_len;
	int cur_len = 0;
	char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
	int received = 0;
	if (total_len >= SCRATCH_BUFSIZE) {
		/* Respond with 500 Internal Server Error */
		httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
		return ESP_FAIL;
	}
	while (cur_len < total_len) {
		received = httpd_req_recv(req, buf + cur_len, total_len);
		if (received <= 0) {
			/* Respond with 500 Internal Server Error */
			httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
			return ESP_FAIL;
		}
		cur_len += received;
	}
	buf[total_len] = '\0';
	cJSON *root = cJSON_Parse(buf);
	int time = cJSON_GetObjectItem(root, "time")->valueint;
	bool enable;
	cJSON *enable_json = cJSON_GetObjectItem(root, "enable");
	if (cJSON_IsTrue(enable_json) == 0) {
		enable = false;
	} else {
		enable = true;
	}
	setPreview(enable, time);
	cJSON_Delete(root);
	httpd_resp_sendstr(req, "Post control values successfully");
	return ESP_OK;

}


/* Handler for getting current color settings */
static esp_err_t color_get_handler(httpd_req_t *req)
{

	int* gOnPtr = getLEDOn();
	int* gOffPtr = getLEDOff();
	int rOn = gOnPtr[0];
	int rOff = gOffPtr[0];
	int gOn = gOnPtr[1];
	int gOff = gOffPtr[1];
	int bOn = gOnPtr[2];
	int bOff = gOffPtr[2];
	int wOn = gOnPtr[3];
	int wOff = gOffPtr[3];

	httpd_resp_set_type(req, "application/json");
	cJSON *root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "rOn", rOn);
	cJSON_AddNumberToObject(root, "gOn", gOn);
	cJSON_AddNumberToObject(root, "bOn", bOn);
	cJSON_AddNumberToObject(root, "wOn", wOn);
	cJSON_AddNumberToObject(root, "rOff", rOff);
	cJSON_AddNumberToObject(root, "gOff", gOff);
	cJSON_AddNumberToObject(root, "bOff", bOff);
	cJSON_AddNumberToObject(root, "wOff", wOff);
	const char *sys_info = cJSON_Print(root);
	httpd_resp_sendstr(req, sys_info);
	free((void *)sys_info);
	cJSON_Delete(root);
	return ESP_OK;
}

/* Handler for getting threshold settings */
static esp_err_t threshold_get_handler(httpd_req_t *req)
{

	int thr = getThreshold();
	httpd_resp_set_type(req, "application/json");
	cJSON *root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "threshold", thr);
	const char *sys_info = cJSON_Print(root);
	httpd_resp_sendstr(req, sys_info);
	free((void *)sys_info);
	cJSON_Delete(root);
	return ESP_OK;
}


/* Handler for getting timing settings */
static esp_err_t timing_get_handler(httpd_req_t *req)
{

	int fadeIn = getFadeIn();
	int fadeOut = getFadeOut();
	int duration = getDuration();

	httpd_resp_set_type(req, "application/json");
	cJSON *root = cJSON_CreateObject();

	cJSON_AddNumberToObject(root, "fadeIn", fadeIn);
	cJSON_AddNumberToObject(root, "fadeOut", fadeOut);
	cJSON_AddNumberToObject(root, "duration", duration);

	const char *sys_info = cJSON_Print(root);
	httpd_resp_sendstr(req, sys_info);
	free((void *)sys_info);
	cJSON_Delete(root);
	return ESP_OK;
}

/* Handler for getting preview settings */
static esp_err_t preview_get_handler(httpd_req_t *req)
{

	int time = getPreviewTime();
	bool enable = getPreviewEnable();

	httpd_resp_set_type(req, "application/json");
	cJSON *root = cJSON_CreateObject();

	cJSON_AddNumberToObject(root, "time", time);
	cJSON_AddBoolToObject(root, "enable", enable);

	const char *sys_info = cJSON_Print(root);
	httpd_resp_sendstr(req, sys_info);
	free((void *)sys_info);
	cJSON_Delete(root);
	return ESP_OK;
}


/* Simple handler for getting system handler */
static esp_err_t system_info_get_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "application/json");
	cJSON *root = cJSON_CreateObject();
	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);
	cJSON_AddStringToObject(root, "version", IDF_VER);
	cJSON_AddNumberToObject(root, "cores", chip_info.cores);
	const char *sys_info = cJSON_Print(root);
	httpd_resp_sendstr(req, sys_info);
	free((void *)sys_info);
	cJSON_Delete(root);
	return ESP_OK;
}

/* Handler for getting current light level */
static esp_err_t ldr_data_get_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "application/json");
	cJSON *root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "measureLight", measureAmbientLight());
	const char *sys_info = cJSON_Print(root);
	httpd_resp_sendstr(req, sys_info);
	free((void *)sys_info);
	cJSON_Delete(root);
	return ESP_OK;
}

esp_err_t start_rest_server(const char *base_path)
{
	REST_CHECK(base_path, "wrong base path", err);
	rest_server_context_t *rest_context = calloc(1, sizeof(rest_server_context_t));
	REST_CHECK(rest_context, "No memory for rest context", err);
	strlcpy(rest_context->base_path, base_path, sizeof(rest_context->base_path));

	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	config.max_uri_handlers = 14;
	config.uri_match_fn = httpd_uri_match_wildcard;

	ESP_LOGI(REST_TAG, "Starting HTTP Server");
	REST_CHECK(httpd_start(&server, &config) == ESP_OK, "Start server failed", err_start);

	/* URI handler for fetching system info */
	httpd_uri_t system_info_get_uri = {
			.uri = "/api/v1/system/info",
			.method = HTTP_GET,
			.handler = system_info_get_handler,
			.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &system_info_get_uri);

	/* URI handler for fetching current color settings */
	httpd_uri_t color_get_uri = {
			.uri = "/read/color",
			.method = HTTP_GET,
			.handler = color_get_handler,
			.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &color_get_uri);

	/* URI handler for fetching threshold settings */
	httpd_uri_t threshold_get_uri = {
			.uri = "/read/threshold",
			.method = HTTP_GET,
			.handler = threshold_get_handler,
			.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &threshold_get_uri);


	/* URI handler for fetching timing settings */
	httpd_uri_t timing_get_uri = {
			.uri = "/read/timing",
			.method = HTTP_GET,
			.handler = timing_get_handler,
			.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &timing_get_uri);

	/* URI handler for fetching preview settings */
	httpd_uri_t preview_get_uri = {
			.uri = "/read/preview",
			.method = HTTP_GET,
			.handler = preview_get_handler,
			.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &preview_get_uri);


	/* URI handler for fetching current light level data */
	httpd_uri_t ldr_data_get_uri = {
			.uri = "/sensor/ldr",
			.method = HTTP_GET,
			.handler = ldr_data_get_handler,
			.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &ldr_data_get_uri);

	/* URI handler for light brightness control */
	httpd_uri_t setup_post_uri = {
			.uri = "/setup/color",
			.method = HTTP_POST,
			.handler = setup_color_handler,
			.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &setup_post_uri);

	/* URI handler for timing control */
	httpd_uri_t timing_post_uri = {
			.uri = "/setup/timing",
			.method = HTTP_POST,
			.handler = set_timing_handler,
			.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &timing_post_uri);

	/* URI handler for threshold control */
	httpd_uri_t threshold_post_uri = {
			.uri = "/setup/threshold",
			.method = HTTP_POST,
			.handler = set_threshold_handler,
			.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &threshold_post_uri);

	/* URI handler for timing control */
	httpd_uri_t preview_post_uri = {
			.uri = "/setup/preview",
			.method = HTTP_POST,
			.handler = set_preview_handler,
			.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &preview_post_uri);

	/* URI handler for getting web server files */
	httpd_uri_t common_get_uri = {
			.uri = "/*",
			.method = HTTP_GET,
			.handler = rest_common_get_handler,
			.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &common_get_uri);

	return ESP_OK;
	err_start:
	free(rest_context);
	err:
	return ESP_FAIL;
}
