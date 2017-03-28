#include <vector>
#include <sstream>
#include <png++/png.hpp>

#include "png.h"

template <typename T>
T GetProperty(v8::Local<v8::Object> object, std::string key) {
    auto value = Nan::Get(object, Nan::New(key).ToLocalChecked()).ToLocalChecked();
    return Nan::To<T>(value).ToChecked();
}

void Png::Init(v8::Local<v8::Object> exports) {
    exports->Set(Nan::New("encode").ToLocalChecked(),
                     Nan::New<v8::FunctionTemplate>(EncodeSync)->GetFunction());

    exports->Set(Nan::New("decode").ToLocalChecked(),
                     Nan::New<v8::FunctionTemplate>(DecodeSync)->GetFunction());
}

void Png::EncodeSync(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    try {
        auto buffer = Nan::To<v8::Object>(info[0]).ToLocalChecked();
        auto width = Nan::To<uint32_t>(info[1]).ToChecked();
        auto height = Nan::To<uint32_t>(info[2]).ToChecked();
        auto palette = v8::Handle<v8::Array>::Cast(info[3]);
        auto alpha = Nan::To<bool>(info[4]).ToChecked();

        uint8_t* pixels = reinterpret_cast<uint8_t*>(node::Buffer::Data(buffer));

        if (width * height > node::Buffer::Length(buffer)) {
            Nan::ThrowError("Not enough data");
            return;
        }

        png::image<png::index_pixel> image(width, height);

        png::palette pal(palette->Length());
        std::vector<png::byte> trns(pal.size());

        for (size_t i = 0; i < pal.size(); ++i) {
            auto color = v8::Handle<v8::Array>::Cast(palette->Get(i));
            auto length = color->Length();

            if (length != 3 && length != 4) {
                // TODO: Error
            }

            auto r = Nan::To<uint32_t>(color->Get(0)).ToChecked();
            auto g = Nan::To<uint32_t>(color->Get(1)).ToChecked();
            auto b = Nan::To<uint32_t>(color->Get(2)).ToChecked();
            auto a = length == 4 ? Nan::To<uint32_t>(color->Get(3)).ToChecked() : 255;

            pal[i] = png::color(r, g, b);
            trns[i] = a;
        }

        image.set_palette(pal);

        if (alpha) {
            image.set_tRNS(trns);
        }

        for (size_t y = 0; y < image.get_height(); ++y) {
            for (size_t x = 0; x < image.get_width(); ++x) {
                image[y][x] = png::index_pixel(*pixels++);
            }
        }

        std::stringstream stream(std::ios_base::out | std::ios_base::binary);
        image.write_stream(stream);

        std::string data = stream.str();
        auto bufferResult = Nan::CopyBuffer(data.c_str(), data.size());
        info.GetReturnValue().Set(bufferResult.ToLocalChecked());
    } catch (png::error& e) {
        Nan::ThrowError(e.what());
    }
}

void Png::DecodeSync(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    try {
        auto buffer = Nan::To<v8::Object>(info[0]).ToLocalChecked();

        std::string str(node::Buffer::Data(buffer), node::Buffer::Length(buffer));
        std::stringstream stream(str, std::ios_base::in | std::ios_base::binary);

        png::image<png::index_pixel> image(stream);

        auto result = Nan::New<v8::Object>();

        auto pixels = std::vector<char>(image.get_width() * image.get_height());

        for (size_t y = 0, i = 0; y < image.get_height(); ++y) {
            for (size_t x = 0; x < image.get_width(); ++x) {
                pixels[i++] = image[y][x];
            }
        }

        auto pal = image.get_palette();
        auto trns = image.get_tRNS();
        bool alpha = trns.size() > 0;

        auto palette = Nan::New<v8::Array>();
        for (size_t i = 0; i < pal.size(); ++i) {
            auto color = Nan::New<v8::Array>();

            color->Set(0, Nan::New(pal[i].red));
            color->Set(1, Nan::New(pal[i].green));
            color->Set(2, Nan::New(pal[i].blue));

            if (alpha) {
                color->Set(3, Nan::New(trns[i]));
            }

            palette->Set(i, color);
        }

        Nan::Set(result, Nan::New("width").ToLocalChecked(), Nan::New(image.get_width()));
        Nan::Set(result, Nan::New("height").ToLocalChecked(), Nan::New(image.get_height()));
        Nan::Set(result, Nan::New("alpha").ToLocalChecked(), Nan::New(alpha));
        Nan::Set(result, Nan::New("pixels").ToLocalChecked(),
                 Nan::CopyBuffer(&pixels[0], pixels.size()).ToLocalChecked());
        Nan::Set(result, Nan::New("palette").ToLocalChecked(), palette);

        info.GetReturnValue().Set(result);
    } catch (png::error& e) {
        Nan::ThrowError(e.what());
    }
}
