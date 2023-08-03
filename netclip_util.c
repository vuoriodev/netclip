#include <httplib.h>
#include <windows.h>
#include <iostream>
#include <cstring>

void setClipboard(const char* output) {
    if (!OpenClipboard(nullptr)) {
        std::cerr << "Cannot open the Clipboard" << std::endl;
        return;
    }

    if (!EmptyClipboard()) {
        std::cerr << "Cannot empty the Clipboard" << std::endl;
        return;
    }

    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, (strlen(output) + 1) * sizeof(char));
    if (!hGlob) {
        std::cerr << "GlobalAlloc() failed" << std::endl;
        return;
    }

    char* lptstrCopy = (char*)GlobalLock(hGlob);
    memcpy(lptstrCopy, output, (strlen(output) + 1) * sizeof(char));
    lptstrCopy[strlen(output) + 1] = (char)0;
    GlobalUnlock(hGlob);

    if (!SetClipboardData(CF_TEXT, hGlob)) {
        std::cerr << "SetClipboardData() failed" << std::endl;
        return;
    }

    CloseClipboard();
}

int main() {
    httplib::Server svr;

    svr.Post("/clipboard", [](const httplib::Request& req, httplib::Response& res) {
        setClipboard(req.body.c_str());
        res.set_content("Text copied to clipboard successfully", "text/plain");
    });

    svr.listen("0.0.0.0", 1234);

    return 0;
}
