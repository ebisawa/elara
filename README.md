# elara

これは、MS-DOS 向け BBS ホストプログラムです。
マルチポートシリアル通信ドライバ MCD が必要です。

mmm/MASH の影響を強く受けた HyperNotes 型の BBS ホストシステムを 10ch 以上の多チャンネルで
稼働させることを目指し、常駐する擬似マルチタスクカーネルが、個別のバイナリとしてビルドされた
デバイスドライバやコマンド群を随時ロードして実行するという、擬似 OS 環境を実現する仕組みだった
ような気がする。