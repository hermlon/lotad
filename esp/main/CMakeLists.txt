idf_component_register(SRCS "esp.c" "wifi_provision.c" "wifi_station.c" "watering/watering.c" "api/api_server.c"
	"api/api_controller.c" "api/api_auth.c" "api/models/password_model.c"
	"watering/water_control.c" "github_ota.c"
	INCLUDE_DIRS "."
	EMBED_TXTFILES github_cert.pem)
