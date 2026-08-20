#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// 搬送装置の設定定数
#define TARGET_SPEED 100    // 目標速度 (mm/s)
#define ACCELERATION 10     // 加速度 (mm/s^2)
#define MAX_ALLOWABLE_SPEED 120 // 異常判定しきい値（過速度）

// 搬送ロボットの状態を管理する構造体
typedef struct {
    int32_t  current_speed;  // 現在の速度 (mm/s)
    int32_t  target_speed;   // 目標速度 (mm/s)
    uint32_t position;       // 現在の位置 (mm)
    bool     emergency_stop; // 緊急停止フラグ (フェイルセーフ)
    char     status_msg[32]; // 状態メッセージ
} transport_robot_t;

/**
 * @brief 搬送ロボットの速度と安全性を制御する関数（1秒ごとの周期処理を想定）
 * @param robot 構造体へのポインタ
 * @param sensor_obstacle_distance 障害物センサーからの距離 (cm)
 */
void update_robot_control(transport_robot_t *robot, int32_t sensor_obstacle_distance) {
    // 1. フェイルセーフ（安全装置）の判定
    if (sensor_obstacle_distance < 30) { // 障害物まで30cm未満なら急停止
        robot->emergency_stop = true;
        robot->current_speed = 0;
        snprintf(robot->status_msg, sizeof(robot->status_msg), "EMERGENCY_STOP_OBSTACLE");
        return;
    }

    if (robot->current_speed > MAX_ALLOWABLE_SPEED) { // 過速度を検知した場合
        robot->emergency_stop = true;
        robot->current_speed = 0;
        snprintf(robot->status_msg, sizeof(robot->status_msg), "EMERGENCY_STOP_OVERSPEED");
        return;
    }

    // 2. スムーズな加減速制御（台形駆動のシミュレート）
    if (robot->current_speed < robot->target_speed) {
        robot->current_speed += ACCELERATION; // 加速
        if (robot->current_speed > robot->target_speed) {
            robot->current_speed = robot->target_speed;
        }
        snprintf(robot->status_msg, sizeof(robot->status_msg), "ACCELERATING");
    } else if (robot->current_speed > robot->target_speed) {
        robot->current_speed -= ACCELERATION; // 減速
        if (robot->current_speed < robot->target_speed) {
            robot->current_speed = robot->target_speed;
        }
        snprintf(robot->status_msg, sizeof(robot->status_msg), "DECELERATING");
    } else {
        snprintf(robot->status_msg, sizeof(robot->status_msg), "STABLE_RUNNING");
    }

    // 3. 位置の更新
    robot->position += robot->current_speed;
}

int main(void) {
    // ロボットの初期化
    transport_robot_t muratec_robot = {
        .current_speed = 0,
        .target_speed = TARGET_SPEED,
        .position = 0,
        .emergency_stop = false
    };

    // シミュレーション用データ：15秒間の「前方障害物との距離(cm)」の変化
    // 13秒目に障害物が急接近するシナリオ
    int32_t mock_sensor_distances[] = {200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 150, 20, 20, 20};
    int total_steps = sizeof(mock_sensor_distances) / sizeof(mock_sensor_distances[0]);

    // CSV形式（データ解析用）でログを出力
    printf("Time(s),Speed(mm/s),Position(mm),Distance(cm),Status\n");
    for (int t = 0; t < total_steps; t++) {
        update_robot_control(&muratec_robot, mock_sensor_distances[t]);
        printf("%d,%d,%d,%d,%s\n", 
               t, muratec_robot.current_speed, muratec_robot.position, 
               mock_sensor_distances[t], muratec_robot.status_msg);
        
        if (muratec_robot.emergency_stop) {
            break; // 安全のためプログラム側でもループを抜ける
        }
    }
    return 0;
}
