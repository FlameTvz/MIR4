// Inclusão de bibliotecas e arquivos de cabeçalho
#include "libs.h"          // Bibliotecas externas ou padrões
#include "vars.h"          // Variáveis globais e configurações
#include "css.h"           // Estilos CSS para a interface do servidor web
WebServer servidorWeb(80); // Criação do objeto do servidor web na porta 80
TFT_eSPI tft = TFT_eSPI();

struct Botao {
    int x, y;             // Posição do botão
    int largura, altura;  // Dimensões do botão
    const char *etiqueta; // Texto do botão
};

// Definição dos botões para controle dos relés
Botao botaoLigar1 = {10, 30, 100, 40, "Ligar Rele 1"};
Botao botaoDesligar1 = {120, 30, 100, 40, "Desligar Rele 1"};
Botao botaoLigar2 = {10, 80, 100, 40, "Ligar Rele 2"};
Botao botaoDesligar2 = {120, 80, 100, 40, "Desligar Rele 2"};
Botao botaoLigar3 = {10, 130, 100, 40, "Ligar Rele 3"};
Botao botaoDesligar3 = {120, 130, 100, 40, "Desligar Rele 3"};
Botao botaoLigar4 = {10, 180, 100, 40, "Ligar Rele 4"};
Botao botaoDesligar4 = {120, 180, 100, 40, "Desligar Rele 4"};

// Função para desenhar um botão
void desenharBotao(Botao botao) {
    
    tft.fillRect(botao.x, botao.y, botao.largura, botao.altura, TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setTextSize(2);
    tft.setCursor(botao.x + 10, botao.y + 10);
    tft.print(botao.etiqueta);
}

// Declaração de funções para manipulação de rotas
void processarRequisicaoControleRele();
void servirPaginaInicial();
void verificarToqueBotao();

// servirPaginaInicial: Envia a página inicial quando a raiz ("/") é acessada
void servirPaginaInicial()
{
    servidorWeb.send(200, "text/html", htmlPaginaInicial);
}
void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin())
  {
    Serial.println("Formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE_TFT))
  {

    if (REPEAT_CAL_TFT)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE_TFT);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE_TFT, "r");
      if (f)
      {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL_TFT)
  {
    // calibration data valid
    tft.setTouch(calData);
  }
  else
  {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Toque nas pontas indicadas");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL_TFT)
    {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibração Completa");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE_TFT, "w");
    if (f)
    {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}
// setup: Executada uma vez na inicialização do microcontrolador
void setup()
{
    
    Serial.begin(115200);         // Inicializa comunicação serial
    pinMode(PINO_RELE_1, OUTPUT); // Configura pino do Relé 1 como saída
    pinMode(PINO_RELE_2, OUTPUT); // Configura pino do Relé 2 como saída
    pinMode(PINO_RELE_3, OUTPUT); // Configura pino do Relé 3 como saída
    pinMode(PINO_RELE_4, OUTPUT); // Configura pino do Relé 4 como saída
    tft.init();
    tft.setRotation(1); // Ajuste conforme a orientação do seu display
    touch_calibrate();
    tft.fillScreen(TFT_BLACK);
    desenharBotao(botaoLigar1);
    desenharBotao(botaoDesligar1);
    desenharBotao(botaoLigar2);
    desenharBotao(botaoDesligar2);
    desenharBotao(botaoLigar3);
    desenharBotao(botaoDesligar3);
    desenharBotao(botaoLigar4);
    desenharBotao(botaoDesligar4);
    
    // Início da conexão WiFi
    Serial.println("Conectando ao WiFi...");
    WiFi.begin(ssid, password); // Inicia conexão WiFi

    // Espera até a conexão WiFi ser estabelecida
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    // Exibe informações da conexão WiFi
    Serial.println("WiFi conectado. Endereço IP: " + WiFi.localIP().toString());

    // Configuração das rotas do servidor web
    servidorWeb.on("/", servirPaginaInicial);                           // Rota para a raiz
    servidorWeb.on("/controlar-rele", processarRequisicaoControleRele); // Rota para controle dos relés
    servidorWeb.begin();
                                                    // Inicia o servidor web
}

// processarRequisicaoControleRele: Controla o estado dos relés via requisições web
void processarRequisicaoControleRele()
{
    Serial.println("Requisição para controle de relé recebida.");

    String idRele = servidorWeb.arg("id");
    if (idRele == "1")
    {
        estadoRele1 = !estadoRele1;
        digitalWrite(PINO_RELE_1, estadoRele1);
        servidorWeb.send(200, "text/plain", "Estado do Relé 1: " + String(estadoRele1));
    }
    else if (idRele == "2")
    {
        estadoRele2 = !estadoRele2;
        digitalWrite(PINO_RELE_2, estadoRele2);
        servidorWeb.send(200, "text/plain", "Estado do Relé 2: " + String(estadoRele2));
    }
    else if (idRele == "3")
    {
        estadoRele3 = !estadoRele3;
        digitalWrite(PINO_RELE_3, estadoRele3);
        servidorWeb.send(200, "text/plain", "Estado do Relé 3: " + String(estadoRele3));
    }
    else if (idRele == "4")
    {
        estadoRele4 = !estadoRele4;
        digitalWrite(PINO_RELE_4, estadoRele4);
        servidorWeb.send(200, "text/plain", "Estado do Relé 4: " + String(estadoRele4));
    }
    else
    {
        servidorWeb.send(400, "text/plain", "ID do Relé inválido");
    }
}

// Função auxiliar para verificar se um botão foi tocado
bool verificarToqueBotao(Botao botao, uint16_t x, uint16_t y) {
    return x >= botao.x && x <= (botao.x + botao.largura) &&
           y >= botao.y && y <= (botao.y + botao.altura);
}


// loop: Executada repetidamente após a inicialização
void loop()
{
servidorWeb.handleClient(); // Processa as requisições recebidas pelo servidor web
    uint16_t x, y;

    if (tft.getTouch(&x, &y)) {
        // Verifica toque em cada botão e controla o relé correspondente
        if (verificarToqueBotao(botaoLigar1, x, y)) digitalWrite(PINO_RELE_1, HIGH);
        if (verificarToqueBotao(botaoDesligar1, x, y)) digitalWrite(PINO_RELE_1, LOW);
        if (verificarToqueBotao(botaoLigar2, x, y)) digitalWrite(PINO_RELE_2, HIGH);
        if (verificarToqueBotao(botaoDesligar2, x, y)) digitalWrite(PINO_RELE_2, LOW);
        if (verificarToqueBotao(botaoLigar3, x, y)) digitalWrite(PINO_RELE_3, HIGH);
        if (verificarToqueBotao(botaoDesligar3, x, y)) digitalWrite(PINO_RELE_3, LOW);
        if (verificarToqueBotao(botaoLigar4, x, y)) digitalWrite(PINO_RELE_4, HIGH);
        if (verificarToqueBotao(botaoDesligar4, x, y)) digitalWrite(PINO_RELE_4, LOW);
    }
}


