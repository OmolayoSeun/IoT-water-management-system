'''mermaid
flowchart TD
    subgraph Local Hardware & Logic
        A[Ultrasonic Sensor] -->|Distance Data| B[ESP8266]
        B --> C[Water Level Calculation]
        C --> D{Water Level}
        
        D -->|Below threshold| E[Activate Pump Relay]
        D -->|Above threshold| F[Deactivate Pump Relay]
    end

    subgraph Cloud & Interface
        B -->|Wi-Fi| G[Blynk Cloud]
        G --> H[Mobile Dashboard]
    end
'''
