/*
 * Copyright (C) 2017 NEC Corporation
 */

#include "necbaas/internal/nb_logger.h"
#include "easylogging++.cc"  //ここでeasyloggingppのソースを取り込んでしまう

INITIALIZE_EASYLOGGINGPP

namespace necbaas {

using std::string;

static NbLogger nb_logger;

NbLogger::NbLogger() {
    // 一律ファイル保存なしに設定
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::ToFile, std::string("false"));
    // FatalログでABORTしない
    el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);

// "necbaas"用の設定
#ifdef NB_DEBUG
    log_config_.setGlobally(el::ConfigurationType::Enabled, std::string("true"));
#else
    log_config_.setGlobally(el::ConfigurationType::Enabled, std::string("false"));
#endif
    log_config_.setGlobally(el::ConfigurationType::ToFile, std::string("false"));
    log_config_.setGlobally(el::ConfigurationType::ToStandardOutput, std::string("true"));
    log_config_.setGlobally(el::ConfigurationType::MillisecondsWidth, std::string("3"));
    log_config_.setGlobally(el::ConfigurationType::PerformanceTracking, std::string("false"));

    log_config_.setGlobally(el::ConfigurationType::Format,
                            std::string("%datetime %level [%logger][%fbase:%line] %msg"));
    log_config_.set(el::Level::Info, el::ConfigurationType::Format, std::string("%datetime %level [%logger]%msg"));

    el::Loggers::reconfigureLogger(LOGGER_ID, log_config_);
}

NbLogger::~NbLogger() {}

void NbLogger::SetDebugLogEnabled(bool flag) {
    string flag_string = flag ? "true" : "false";
    nb_logger.log_config_.set(el::Level::Trace, el::ConfigurationType::Enabled, flag_string);
    nb_logger.log_config_.set(el::Level::Debug, el::ConfigurationType::Enabled, flag_string);
    nb_logger.log_config_.set(el::Level::Verbose, el::ConfigurationType::Enabled, flag_string);

    el::Loggers::reconfigureLogger(LOGGER_ID, nb_logger.log_config_);
}

void NbLogger::SetErrorLogEnabled(bool flag) {
    string flag_string = flag ? "true" : "false";
    nb_logger.log_config_.set(el::Level::Fatal, el::ConfigurationType::Enabled, flag_string);
    nb_logger.log_config_.set(el::Level::Error, el::ConfigurationType::Enabled, flag_string);
    nb_logger.log_config_.set(el::Level::Warning, el::ConfigurationType::Enabled, flag_string);

    el::Loggers::reconfigureLogger(LOGGER_ID, nb_logger.log_config_);
}

void NbLogger::SetRestLogEnabled(bool flag) {
    string flag_string = flag ? "true" : "false";
    nb_logger.log_config_.set(el::Level::Info, el::ConfigurationType::Enabled, flag_string);

    el::Loggers::reconfigureLogger(LOGGER_ID, nb_logger.log_config_);
}

bool NbLogger::IsDebugLogEnabled() {
    el::Logger* l = el::Loggers::getLogger(LOGGER_ID);
    return l->typedConfigurations()->enabled(el::Level::Trace);
}

bool NbLogger::IsErrorLogEnabled() {
    el::Logger* l = el::Loggers::getLogger(LOGGER_ID);
    return l->typedConfigurations()->enabled(el::Level::Error);
}

bool NbLogger::IsRestLogEnabled() {
    el::Logger* l = el::Loggers::getLogger(LOGGER_ID);
    return l->typedConfigurations()->enabled(el::Level::Info);
}
}  // namespace necbaas
