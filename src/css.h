const char* htmlPaginaInicial = R"html(
<!DOCTYPE html>
<html lang="pt">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Controle de Relés</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            background-color: #f0f0f0;
            color: #333;
            text-align: center;
        }
        .container {
            padding: 20px;
        }
        .btn-toggle {
            position: relative;
            width: 60px;
            height: 30px;
            margin: 10px;
            border-radius: 30px;
            background-color: #ddd;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        .btn-toggle:before {
            content: "";
            position: absolute;
            top: 3px;
            left: 3px;
            width: 24px;
            height: 24px;
            border-radius: 50%;
            background-color: white;
            transition: all 0.3s;
        }
        .btn-toggle.active {
            background-color: #4CAF50;
        }
        .btn-toggle.active:before {
            transform: translateX(30px);
        }
        .rele-label {
            display: inline-block;
            margin-right: 10px;
            font-weight: bold;
        }
    </style>
</head>

<body>
    <div class="container">
        <h1>Controle de Relés</h1>
        <div id="reles">
            <!-- Botões deslizantes e rótulos para 4 relés -->
            <div class="rele-label">Relé 1:</div>
            <div class="btn-toggle" onclick="toggleRele(1, this)"></div><br>
            <div class="rele-label">Relé 2:</div>
            <div class="btn-toggle" onclick="toggleRele(2, this)"></div><br>
            <div class="rele-label">Relé 3:</div>
            <div class="btn-toggle" onclick="toggleRele(3, this)"></div><br>
            <div class="rele-label">Relé 4:</div>
            <div class="btn-toggle" onclick="toggleRele(4, this)"></div>
        </div>
    </div>

    <script>
    function toggleRele(numeroRele, element) {
        element.classList.toggle('active');
        console.log('Alternar Relé ' + numeroRele);

        // Envia a requisição ao servidor para controlar o relé
        fetch('/controlar-rele?id=' + numeroRele)
            .then(response => {
                if (!response.ok) {
                    throw new Error('Falha na requisição');
                }
                return response.text();
            })
            .then(text => {
                console.log(text);  // Exibe a resposta do servidor no console
            })
            .catch(error => {
                console.error('Erro na requisição:', error);
            });
    }
</script>

</body>

</html>
)html";
