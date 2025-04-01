## ハイブリッドシステムで内蔵フラッシュに設定を記憶させるTIPS
<br>
<br>
★まえがき
<br>
<br>
try-kernelとpico-sdkを混在させるハイブリッドシステムで開発しています。
<br>
設定データをフラッシュに書き込むのは通常のマイコンだと内蔵データフラッシュがあったりしますね。
<br>
picoはSPIフラッシュが内蔵されており、2MBも容量があるので、その一部をデータ領域に使えそうです。
<br>
SPIフラッシュは「W25Q16JVUXIQ」という型式で、容量は2M、アドレス: 0x000000 ～ 0x1FFFFFです。
<br>
<img width="320" alt="image" src="https://github.com/user-attachments/assets/414c197b-884e-4bb6-a89d-3e8e48061ade" />
<br>
<br>
<br>
<br>
ただし2MBすべて使えるわけでなく、ROMエリア(0x000000~0x0FFFFF)とXIP(0x100000~0x1FFFFF)エリアで分かれています。
<img width="692" alt="image" src="https://github.com/user-attachments/assets/3697b03b-6e9a-4fc5-be07-b1dad9d5130f" />
<br>
<br>
■ROMとXIPの話
<br>
ROMは読み出し専用のメモリです。主に、ブートローダーと呼ばれる、Pico起動時の初期化処理を行うプログラムが格納されています。
<br>
ユーザーがROMにプログラムを書き込むことはできません。
<br>
<br>
続いてXIP領域ですが、この領域をプログラムコードやデータを格納のために使用できます。
<br>
XIP（eXecute In Place）は外部接続されたフラッシュメモリの領域を、CPUが直接実行可能なメモリ空間として扱う技術のことです。
<br>
フラッシュメモリ上のプログラムコードをRAMにコピーせずに、直接実行できるので高速動作などのメリットがあります。
<br>
<br>
■ROMとXIPの起動イメージの話
<br>
Pico起動時、CPUはROMに格納されたブートローダーを実行します。ブートローダーは、フラッシュメモリの初期化やXIPモードの設定
<br>
などを行い、その後にXIP領域に格納されたユーザープログラムを実行します。ROMはPicoの起動処理を行い、XIPはユーザープログラム
<br>
の実行領域として機能します。
<br>ROMはPicoの起動に必要なプログラムが格納された読み出し専用メモリで、XIPはユーザプログラムやデータを格納できるエリアです。
<br>
<br>
★はまったところ
<br>
フラッシュ書き込みをフルスクラッチで作るのは時間がかかるので、pico-sdkのAPIを検討しました。
<br>
具体的には、以下のAPIで実装できます。
<br>
<img width="467" alt="image" src="https://github.com/user-attachments/assets/479b4927-4367-43f4-9a32-9e23684e4ef5" />
<br>
-----<br>
■flash_range_erase<br>
・機能：指定されたアドレス範囲のフラッシュメモリを消去します。<br>
・注意：フラッシュメモリは、書き込み前に消去する必要があります。<br>
・引数：offset: 消去を開始するフラッシュメモリのアドレス（オフセット）　count: 消去するバイト数<br>
・備考：フラッシュメモリは、セクタ単位で消去されます。セクタサイズは通常4KBです。<br>
　　　　countは、セクタサイズの倍数である必要があります。<br>
-----<br>
■flash_range_program<br>
・機能：指定されたアドレス範囲のフラッシュメモリにデータを書き込みます。<br>
・引数：offset: 書き込みを開始するフラッシュメモリのアドレス　src: 書き込むデータのポインタ　count: 書き込むバイト数<br>
・備考：フラッシュメモリは、ページ単位で書き込まれます。ページサイズは通常256バイトです。<br>
-----<br>
<br>
<br>
■マルチコアならではの問題
<br>
Raspberry Pi Picoのフラッシュメモリへの書き込み中に、別のコアがプログラムコードを含むフラッシュ領域にアクセスすると、<br>
エラーが発生します。例外ハンドラへ飛びます。<br>
<br>
SPIフラッシュへの書き込み動作中は、CPUコアは命令をフェッチしたり、データを読み出したりすることができません。<br>
同じページなどではなく、フラッシュそのものへのアクセスもエラーとなります。<br>
Picoのマルチコア環境では、core0とcore1が独立して動作するため、片方のコアがフラッシュ書き込みを行っている間にもう片方のコアが<br>
動作することは普通にありますが、上記の理由でエラーとなります。<br>
<br>
■対策
<br>
対策としてはフラッシュ書き込み中に割り込みの禁止、そして片方のコアを停止させます。
<br>
<img width="488" alt="image" src="https://github.com/user-attachments/assets/d014d6d2-786d-4c8e-a4f5-b9aa282b9083" />
<br>
multicore_lockout_start_blocking　と　multicore_lockout_end_blocking　でサンドイッチします。<br>
<br>
またこの関数を使う前に、事前に停止させたいコアで　multicore_lockout_victim_init　をコールして<br>
他のコアによって強制的に一時停止させることができる状態にします。<br>
<img width="386" alt="image" src="https://github.com/user-attachments/assets/09545ae0-c644-4717-9b11-784744394649" />
<br>
<br>
TIPSは以上になります。

