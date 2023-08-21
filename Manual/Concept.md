﻿```mermaid
classDiagram
    class App {
        +Create() void
        +Destroy() void
        +Update() void
    }
    
    App <|-- Application : CreateApplication()
    
    class ApplicationData {
        +string Name
        +string Resolution
    }

    class Application {
        <<abstract>>
        +Create()* void
        +Destroy()* void
        +Update()* void
        -Application mInstance$
        -ApplicationData mData
    }

    Application "1" <.. ApplicationData
    
    Application <--> Config
    Application <--> Context
    Application <--> Window
    
    class Config {
    }

    class Context {
    }
    
    class Window {
    }
```