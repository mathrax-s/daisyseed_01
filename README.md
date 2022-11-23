# daisyseed_01

ElectroSmithのDaisySeedのプログラムです。</br>
ArduinoのDaisyDuinoライブラリを使っています。

DaisySeed
https://www.electro-smith.com/daisy/daisy

DaisyDuino
https://github.com/electro-smith/DaisyDuino

## 説明
シリアル通信（115200bps）で「z x c b n m , . /」を受信して、ドレミファソラシドを奏でます。</br>
10コの和音を想定し、オシレータとエンベロープを10コずつ持ち、最後にリバーブをかけています。

## サウンド
以下の動画は、このプログラムで音を鳴らして、PCに取り込んだものです。DaisySeedの音のままで、加工はしていません。</br>

https://user-images.githubusercontent.com/34113514/203465543-f04c72ea-815b-4f16-b26c-33831b839c9f.mp4

