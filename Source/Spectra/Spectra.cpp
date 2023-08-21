﻿#include <Settings.h>
#include <Ultra/EntryPoint.h>

import Ultra;

namespace Ultra {

using namespace Ultra::UI;

// Application
class Spectra: public Application {
public:
    // Constructors and Destructor
    Spectra(const ApplicationProperties &properties): Application(properties) {}
    ~Spectra() = default;

    // Methods
    void Create() {
        mRenderer = Renderer::Create();
        Resource::Instance();
    }

    void Destroy() {}

    void Update([[maybe_unused]] Timestamp deltaTime) {
        HmGui::ShowDemo(deltaTime);
        mRenderer->RenderFrame();
        HmGui::Draw();
    }

    // Styles
    string AsciiLogo() {
        return 1 + R"(
      ____ ___.__   __                     _________                    __                 
     |    |   \  |_/  |_____________      /   _____/_____   ____  _____/  |_____________   
     |    |   /  |\   __\_  __ \__  \     \_____  \\____ \_/ __ \/ ___\   __\_  __ \__  \  
     |    |  /|  |_|  |  |  | \// __ \_   /        \  |_> >  ___|  \___|  |  |  | \// __ \_
     |______/ |____/__|  |__|  (____  /  /_______  /   __/ \___  >___  >__|  |__|  (____  /
                                    \/           \/|__|        \/    \/                 \/ )";
    }

private:
    Scope<Renderer> mRenderer;
};

// Application Entry-Point
Application *CreateApplication() {
    return new Spectra({ "Spectra", "1280x1024", GraphicsAPI::OpenGL });
}

}