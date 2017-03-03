/*
 * COPYRIGHT (C) 2017 NEC CORPORATION
 *
 * ALL RIGHTS RESERVED BY NEC CORPORATION,  THIS PROGRAM
 * MUST BE USED SOLELY FOR THE PURPOSE FOR WHICH IT WAS
 * FURNISHED BY NEC CORPORATION,  NO PART OF THIS PROGRAM
 * MAY BE REPRODUCED OR DISCLOSED TO OTHERS,  IN ANY FORM
 * WITHOUT THE PRIOR WRITTEN PERMISSION OF NEC CORPORATION.
 *
 * NEC CONFIDENTIAL AND PROPRIETARY
 */

#include "necbaas/internal/nb_http_file_download_handler.h"
#include <thread>
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;

NbHttpFileDownloadHandler::NbHttpFileDownloadHandler(const string &file_name) {
    file_stream_.open(file_name, std::ios::out | std::ios::binary | std::ios::trunc);
}

NbHttpFileDownloadHandler::~NbHttpFileDownloadHandler() {}

bool NbHttpFileDownloadHandler::IsError() const { return !file_stream_; }

size_t NbHttpFileDownloadHandler::WriteCallback(char *buffer, size_t size, size_t nmemb) {
    size_t write_size = size * nmemb;

    if (GetStatusCode() != 200) {
        // エラーの場合は基底クラスを実行
        NBLOG(ERROR) << "response failed code: " << tmp_status_code_;
        return NbHttpHandler::WriteCallback(buffer, size, nmemb);
    } else {
        file_stream_.write(buffer, write_size);
        file_stream_.flush();
        if (IsError()) {
            // 書込みエラーのため処理中断
            NBLOG(ERROR) << "file_stream: write error";
            return 0;
        }
    }

    return write_size;
}

int NbHttpFileDownloadHandler::GetStatusCode() {
    if (tmp_status_code_ == 0) {
        ParseStatusLine(&tmp_status_code_, nullptr);
    }
    return tmp_status_code_;
}
}  // namespace necbaas
