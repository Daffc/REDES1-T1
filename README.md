<h1>TRABALHO 1</h1>
Implementar um FPT simplificado entre 2 máquinas conectadas diretamente atravez de um cabo "crossover" usando RAW Socket seguindo o potocolo criado em sala de aula.

<h2>PROTOCOLO</h2>
Kermit

<h2>LLC</h2>
<ul>
    <li>Enquadramento</li>
    <li>Sequencialização</li>
    <li>Controle de Fluxo
        <ul>
            <li>Mensagem de controle (Para-e-espera 1 mensagem)</li>
            <li>Dados (Para e espera 3 mensagens)</li>
        </ul>
    </li>
    <li>Detecção de erros (CRC 8bits)</li>
</ul>

<h2>MENSAGEM</h2>
<table>
  <tr>
    <th>Marcador de início</th>
    <th>Tamanho</th>
    <th>Sequência</th>
    <th>Tipo</th>
    <th>Dados</th>
    <th>CRC</th>
  </tr>
  <tr>
    <td>8 bits</td>
    <td>7 bits</td>
    <td>5 bits</td>
    <td>4 bits</td>
    <td>0 à 127 Bytes</td>
    <td>8 bits</td>
  </tr>
</table>
<ul>
    <li>Marcador de Início
        <ul>
        <li>01111110</li>
    </ul>
    </li>
    <li>Tamanho
    <ul>
        <li>Quantidade de Bytes do campo DAdos</li>
    </ul>
    </li>
</ul>

<h2>COMANDOS OBRIGATORIOS</h2>
<h3>Comandos Locais:</h3>
<ul>
    <li>cd</li>
    <li>ls</li>
</ul>

<h3>Comandos Remotos:</h3>
<ul>
    <li>cd</li>
    <li>ls (-l | -a | -la)</li>
    <li>ls (-l | -a | -la)</li>
    <li>PUT</li>
    <li>GET</li>
</ul>

<h2>CÓDIGOS RELEVANTES</h2>
<h3>Códigos das instruções</h3>
<ol start="0">
    <li>NACK</li>
    <li>ACK</li>
    <li>OK</li>
    <li>DADOS</li>
    <li>---</li>
    <li>DESCRITOR DE ARQUIVOS</li>
    <li>---</li>
    <li>MOSTRAR NA TELA</li>
    <li>---</li>
    <li>---</li>
    <li>GET</li>
    <li>PUT</li>
    <li>CD</li>
    <li>LS</li>
    <li>ERRO</li>
    <li>FIM</li>
</ol>
<h3>Código de erro (unsigned char): </h3>
<ol>
    <li>INEXISTENTE</li>
    <li>PERMISSÃO</li>
    <li>ESPAÇO INSUFICIENTE</li>
</ol>

<h2>ENTREGA</h2>
<ul>
    <li><b>Valor</b> 
        <ul>
             <li>3 Pontos</li>
        </ul>
    </li>
    <li><b>Entrega</b> 
        <ul>
             <li>27/11 -> 110% da nota </li>
             <li>29/11 -> 90% da nota </li>
        </ul>
    </li>
        <li><b>Relatório</b> 
        <ul>
             <li>Impresso</li>
        </ul>
    </li>
        <li><b>Código</b> 
        <ul>
             <li>E-mail</li>
        </ul>
    </li>
</ul>