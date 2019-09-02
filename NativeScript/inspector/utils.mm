#include <MobileCoreServices/MobileCoreServices.h>
#include <Foundation/Foundation.h>
#include "utils.h"
#include <codecvt>
#include <locale>

std::string v8_inspector::GetMIMEType(std::string filePath) {
    NSString* nsFilePath = [NSString stringWithUTF8String:filePath.c_str()];
    NSString* fullPath = [nsFilePath stringByExpandingTildeInPath];

    NSFileManager *fileManager = [NSFileManager defaultManager];
    BOOL isDirectory;
    if (![fileManager fileExistsAtPath:fullPath isDirectory:&isDirectory] || isDirectory) {
        return std::string();
    }

    NSString* fileExtension = [fullPath pathExtension];
    CFStringRef uti = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, (__bridge CFStringRef)fileExtension, nil);
    if (uti == nil) {
        return std::string();
    }

    NSString* mimeType = (__bridge NSString*)UTTypeCopyPreferredTagWithClass(uti, kUTTagClassMIMEType);
    if (mimeType == nil) {
        return std::string();
    }

    std::string result = [mimeType UTF8String];
    return result;
}

std::string v8_inspector::ToStdString(const StringView& value) {
    std::vector<uint16_t> buffer(value.length());
    for (size_t i = 0; i < value.length(); i++) {
        if (value.is8Bit()) {
            buffer[i] = value.characters8()[i];
        } else {
            buffer[i] = value.characters16()[i];
        }
    }

    std::u16string value16(buffer.begin(), buffer.end());

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::string result = convert.to_bytes(value16);

    return result;
}
