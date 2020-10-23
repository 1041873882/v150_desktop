#include "sys.h"
#include "sMisc.h"
#include "cJSON.h"
#include "HttpRequest.h"
#include "AdvanceWirelessHttp.h"

AdvanceWirelessHttp awtek_http;

int response_callback(int code, std::string response)
{
	printf("response code:%d\n", code);
	if (code == 200) {
		const char *res = response.c_str();
		cJSON *root, *result, *message;
		root = cJSON_Parse(res);
		if (root == NULL) {
			if (strstr(res, "OK")) {
				printf("push success\n");
				return 0;
			} else {
				printf("%s\n", res);
				return -1;
			}
		}
		result = cJSON_GetObjectItem(root, "result");
		if (result == NULL) {
			return -1;
		}
		if (result->valueint == true) {
			printf("register success\n");
			return 0;
		} else {
			printf("%s\n", res);
			return -1;
		}
	} else {
		printf("send error\n");
	}
	return -1;
}

AdvanceWirelessHttp::AdvanceWirelessHttp()
{
}

AdvanceWirelessHttp::~AdvanceWirelessHttp()
{
}

int AdvanceWirelessHttp::send_message(int type, int io, int sensor)
{
	int ret = -1;
	dict headers;
	char pid[128];
	char url[256];
	snprintf(pid, sizeof(pid), "%s-%s", sys.awtek.community_code(), sys.awtek.id_code());
	if (type == REGISTER) {
		cJSON *root = cJSON_CreateObject();
		if (NULL == root) {
			return -1;
		}
		cJSON_AddStringToObject(root, "ResidentID", pid);
		cJSON_AddBoolToObject(root, "CheckConnection", true);
		cJSON_AddStringToObject(root, "InstanceID", pid);
		char *body = cJSON_Print(root);
		if (NULL == body) {
			cJSON_Delete(root);
			return -1;
		}
		headers.insert(make_pair("Content-Type", "application/json"));
		http_request.post(sys.awtek.url(0), &headers, body, response_callback);
		free(body);
		cJSON_Delete(root);
		return 0;
	} else if (type == ALARM) {
		printf("type == ALARM\n");
		snprintf(url, sizeof(url), "%s?pid=%s&sensor=%d&io=%d", sys.awtek.url(1), pid, sensor, io);
		headers.insert(make_pair("Content-Type", "application/json"));
		return http_request.get(url, &headers, NULL, response_callback);
	}
	return -1;
}
