import pandas as pd 

FILENAME = "../bin/性能测试.csv"

if __name__ == "__main__":
    df = pd.read_csv(FILENAME)

    df.set_index("轮次编号", inplace=True)

    summary = []

    for round_num, data in df.groupby(df.index):
        total_execution_count = data['总执行次数'].sum()  # 总执行次数
        total_success_count = data['成功次数'].sum()  # 成功次数
        total_failure_count = data['失败次数'].sum()  # 失败次数
        avg_duration = data['平均耗时(ms)'].mean()  # 平均耗时 (ms)
        max_duration = data['最大耗时(ms)'].max()  # 最大耗时 (ms)
        min_duration = data['最小耗时(ms)'].min()  # 最小耗时 (ms)
        total_duration = data['总耗时(s)'].sum()  # 总耗时 (s)
        throughput = total_execution_count / total_duration if total_duration > 0 else 0  # 吞吐量 (tps)
        success_rate = (total_success_count / total_execution_count) * 100 if total_execution_count > 0 else 0  # 成功率
        error_rate = (total_failure_count / total_execution_count) * 100 if total_execution_count > 0 else 0  # 错误率  

        summary.append({
            '轮次编号': round_num,
            '总执行次数': total_execution_count,
            '成功次数': total_success_count,
            '失败次数': total_failure_count,
            '平均耗时(ms)': avg_duration,
            '最大耗时(ms)': max_duration,
            '最小耗时(ms)': min_duration,
            '总耗时(s)': total_duration,
            '吞吐量(tps)': throughput,
            '成功率': success_rate,
            '错误率': error_rate
        })    
        
    summary_df = pd.DataFrame(summary)    
    summary_df.to_csv("./report.csv", index=False)
