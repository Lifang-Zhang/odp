/* Copyright (c) 2021, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#include <odp_api.h>
#include <odp/helper/odph_api.h>

static int test_default_cli_param(odp_instance_t instance)
{
	odph_cli_param_t cli_param;

	odph_cli_param_init(&cli_param);

	if (odph_cli_init(instance, &cli_param)) {
		ODPH_ERR("Error: odph_cli_init() failed.\n");
		return -1;
	}

	if (odph_cli_start()) {
		ODPH_ERR("Error: odph_cli_start() failed.\n");
		return -1;
	}

	if (odph_cli_stop()) {
		ODPH_ERR("Error: odph_cli_stop() failed.\n");
		return -1;
	}

	if (odph_cli_term()) {
		ODPH_ERR("Error: odph_cli_term() failed.\n");
		return -1;
	}

	return 0;
}

static int server_init_fn_test(void *arg)
{
	char *arg_str = (char *)arg;

	if (strcmp(arg_str, "arg-init") != 0) {
		ODPH_ERR("Error: %s failed.\n", __func__);
		return -1;
	}

	printf("server_init_fn arg: %s\n", arg_str);
	return 0;
}

static int server_term_fn_test(void *arg)
{
	char *arg_str = (char *)arg;

	if (strcmp(arg_str, "arg-term") != 0) {
		ODPH_ERR("Error: %s failed.\n", __func__);
		return -1;
	}

	printf("server_term_fn arg: %s\n", arg_str);
	return 0;
}

static int test_cli_param(odp_instance_t instance)
{
	odph_cli_param_t cli_param;

	odph_cli_param_init(&cli_param);

	char init_fn_arg[10] = "arg-init";
	char term_fn_arg[10] = "arg-term";

	cli_param.hostname = "cli-test";
	cli_param.server_init_fn = server_init_fn_test;
	cli_param.server_init_fn_arg = init_fn_arg;
	cli_param.server_term_fn = server_term_fn_test;
	cli_param.server_term_fn_arg = term_fn_arg;

	if (odph_cli_init(instance, &cli_param)) {
		ODPH_ERR("Error: odph_cli_init() failed.\n");
		return -1;
	}

	if (odph_cli_start()) {
		ODPH_ERR("Error: odph_cli_start() failed.\n");
		return -1;
	}

	if (odph_cli_stop()) {
		ODPH_ERR("Error: odph_cli_stop() failed.\n");
		return -1;
	}

	if (odph_cli_term()) {
		ODPH_ERR("Error: odph_cli_term() failed.\n");
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	odp_instance_t instance;
	odph_helper_options_t helper_options;
	odp_init_t init_param;
	int ret = 0;

	argc = odph_parse_options(argc, argv);
	if (odph_options(&helper_options)) {
		ODPH_ERR("Error: reading ODP helper options failed.\n");
		exit(EXIT_FAILURE);
	}

	odp_init_param_init(&init_param);
	init_param.mem_model = helper_options.mem_model;

	memset(&instance, 0, sizeof(instance));

	if (odp_init_global(&instance, NULL, NULL)) {
		ODPH_ERR("Error: ODP global init failed.\n");
		exit(EXIT_FAILURE);
	}

	if (odp_init_local(instance, ODP_THREAD_CONTROL)) {
		ODPH_ERR("Error: ODP local init failed.\n");
		exit(EXIT_FAILURE);
	}

	ret += test_default_cli_param(instance);
	ret += test_cli_param(instance);

	if (odp_term_local()) {
		ODPH_ERR("Error: ODP local term failed.\n");
		exit(EXIT_FAILURE);
	}

	if (odp_term_global(instance)) {
		ODPH_ERR("Error: ODP global term failed.\n");
		exit(EXIT_FAILURE);
	}

	return ret;
}
