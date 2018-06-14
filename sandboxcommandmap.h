#ifndef SANDBOXCOMMANDMAP_H
#define SANDBOXCOMMANDMAP_H


#include <QMap>

static QMap<QString,QString> g_sandBoxCommandMap;

void initSandBoxCommandMap()
{
    g_sandBoxCommandMap.insert("open",      "sandbox_open");
    g_sandBoxCommandMap.insert("close",     "sandbox_close");
    g_sandBoxCommandMap.insert("register_contract", "sandbox_register_contract");
    g_sandBoxCommandMap.insert("call_contract",     "sandbox_call_contract");
    g_sandBoxCommandMap.insert("upgrade_contract",  "sandbox_upgrade_contract");
    g_sandBoxCommandMap.insert("upgrade_contract_testing",  "sandbox_upgrade_contract_testing");
    g_sandBoxCommandMap.insert("destroy_contract",  "sandbox_destroy_contract");
    g_sandBoxCommandMap.insert("destroy_contract_testing",  "sandbox_destroy_contract_testing");
    g_sandBoxCommandMap.insert("wallet_transfer_to_contract",   "sandbox_transfer_to_contract");
    g_sandBoxCommandMap.insert("get_contract_info",         "sandbox_get_contract_info");
    g_sandBoxCommandMap.insert("get_contract_balance",      "sandbox_get_contract_balance");
    g_sandBoxCommandMap.insert("wallet_account_balance",    "sandbox_account_balance");
    g_sandBoxCommandMap.insert("register_contract_testing", "sandbox_register_contract_testing");
    g_sandBoxCommandMap.insert("call_contract_testing",     "sandbox_call_contract_testing");
    g_sandBoxCommandMap.insert("wallet_transfer_to_contract_testing",   "sandbox_transfer_to_contract_testing");
    g_sandBoxCommandMap.insert("compile_contract",  "sandbox_compile_contract");
    g_sandBoxCommandMap.insert("wallet_list_my_addresses",  "sandbox_list_my_addresses");
//    g_sandBoxCommandMap.insert("list_event_handler",  "sandbox_list_event_handler");
    g_sandBoxCommandMap.insert("get_contract_info_from_gpc_file",   "get_contract_info_from_gpc_file");
    g_sandBoxCommandMap.insert("wallet_get_contracts",      "wallet_get_contracts");
    g_sandBoxCommandMap.insert("wallet_scan_contracts",     "wallet_scan_contracts");
    g_sandBoxCommandMap.insert("wallet_delegate_set_block_production",  "wallet_delegate_set_block_production");
    g_sandBoxCommandMap.insert("blockchain_get_all_contracts",          "blockchain_get_all_contracts");
    g_sandBoxCommandMap.insert("blockchain_get_forever_contracts",      "blockchain_get_forever_contracts");
    g_sandBoxCommandMap.insert("about",     "about");
    g_sandBoxCommandMap.insert("info",      "info");
    g_sandBoxCommandMap.insert("help",      "help");
    g_sandBoxCommandMap.insert("blockchain_get_info",   "blockchain_get_info");
    g_sandBoxCommandMap.insert("wallet_get_info",       "wallet_get_info");
    g_sandBoxCommandMap.insert("execute_command_line",  "execute_command_line");
    g_sandBoxCommandMap.insert("execute_script",        "execute_script");
    g_sandBoxCommandMap.insert("ntp_update_time",       "ntp_update_time");
    g_sandBoxCommandMap.insert("disk_usage",            "disk_usage");
    g_sandBoxCommandMap.insert("network_add_node",      "network_add_node");
    g_sandBoxCommandMap.insert("network_get_connection_count",  "network_get_connection_count");
    g_sandBoxCommandMap.insert("network_get_peer_info", "network_get_peer_info");
    g_sandBoxCommandMap.insert("network_get_info",      "network_get_info");
    g_sandBoxCommandMap.insert("network_list_potential_peers",  "network_list_potential_peers");
    g_sandBoxCommandMap.insert("network_get_blocked_ips",       "network_get_blocked_ips");

};


#endif // SANDBOXCOMMANDMAP_H
