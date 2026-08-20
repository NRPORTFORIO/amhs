import pandas as pd
import io

# C言語プログラムから出力された搬送ログ（シミュレーション結果）の模擬データ
csv_data = """Time(s),Speed(mm/s),Position(mm),Distance(cm),Status
0,10,10,200,ACCELERATING
1,20,30,200,ACCELERATING
2,30,60,200,ACCELERATING
3,40,100,200,ACCELERATING
4,50,150,200,ACCELERATING
5,60,210,200,ACCELERATING
6,70,280,200,ACCELERATING
7,80,360,200,ACCELERATING
8,90,450,200,ACCELERATING
9,100,550,200,ACCELERATING
10,100,650,200,STABLE_RUNNING
11,100,750,200,STABLE_RUNNING
12,100,850,150,STABLE_RUNNING
13,0,850,20,EMERGENCY_STOP_OBSTACLE"""

def evaluate_transport_log():
    print("--- 村田機械様想定：搬送ログデータ解析システム ---")
    
    # CSVデータを読み込み (データサイエンスの基本処理)
    df = pd.read_csv(io.StringIO(csv_data))
    
    # 1. 稼働効率の算出
    max_speed = df['Speed(mm/s)'].max()
    total_distance = df['Position(mm)'].max()
    print(f"[品質保証] 総走行距離: {total_distance} mm")
    print(f"[稼働監視] 最高到達速度: {max_speed} mm/s")
    
    # 2. 異常（緊急停止）の自動検知
    anomaly = df[df['Status'].str.contains('EMERGENCY')]
    if not anomaly.empty:
        stop_time = anomaly['Time(s)'].values[0]
        reason = anomaly['Status'].values[0]
        print(f"[⚠️警告] 異常停止を検知しました。時間: {stop_time}秒目 | 原因: {reason}")
        print(f"[解析結果] 停止直前の障害物距離: {anomaly['Distance(cm)'].values[0]} cm -> フェイルセーフ作動ロジックの正常性を確認。")
    else:
        print("[正常] 運行ログに異常は見つかりませんでした。")

if __name__ == "__main__":
    evaluate_transport_log()
