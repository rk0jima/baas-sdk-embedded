/*
 * Copyright (C) 2017 NEC Corporation
 */

#include "necbaas/internal/nb_http_file_upload_handler.h"
#include <thread>
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;

NbHttpFileUploadHandler::NbHttpFileUploadHandler(const string &file_name) {
    file_stream_.open(file_name, std::ios::in | std::ios::binary);
}

NbHttpFileUploadHandler::~NbHttpFileUploadHandler() {}

bool NbHttpFileUploadHandler::IsError() const { return !file_stream_; }

size_t NbHttpFileUploadHandler::ReadCallback(void *buffer, size_t size, size_t nmemb) {
    if (file_stream_.eof()) {
        // ファイル終端なら転送終了
        return 0;
    }

    if (IsError()) {
        // エラー発生
        NBLOG(ERROR) << "file_stream: read error";
        return 0;
    }

    size_t read_size = (size_t)file_stream_.read((char *)buffer, size * nmemb).gcount();
    if (read_size <= 0) {
        // 読み込み失敗
        NBLOG(ERROR) << "file_stream: read error";
        return 0;
    }

    return read_size;
}
}  // namespace necbaas
