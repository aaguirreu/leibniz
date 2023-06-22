import re
import pandas as pd
import numpy as np

def parse_txt_to_csv(txtfile):
    parallel_time_table = {i: [np.nan]*10 for i in range(1, 21)}
    speedup_table = {i: [np.nan]*10 for i in range(1, 21)}
    efficiency_table = {i: [np.nan]*10 for i in range(1, 21)}
    sequential_time_table = {1: [np.nan]*10}
    parallel_approximation_table = {i: [np.nan]*10 for i in range(1, 21)}
    sequential_approximation_table = {1: [np.nan]*10}

    with open(txtfile, 'r') as file:
        for line in file:
            if "Hilo" in line:
                brackets = re.findall(r'\[(.*?)\]', line)
                if len(brackets) >= 2:
                    thread = int(brackets[0])
                    iteration = int(brackets[1]) - 1
                    value = float(re.findall(r'[-+]?\d*\.\d+|\d+', line)[-1])
                    if "Tiempo de ejecucion paralelo" in line:
                        parallel_time_table[thread][iteration] = value
                    elif "Speedup" in line:
                        speedup_table[thread][iteration] = value
                    elif "Eficiencia" in line:
                        efficiency_table[thread][iteration] = value
                    elif "Aproximacion paralela de pi" in line:
                        parallel_approximation_table[thread][iteration] = value
            elif "Tiempo de ejecucion secuencial" in line:
                brackets = re.findall(r'\[(.*?)\]', line)
                if brackets:
                    iteration = int(brackets[0]) - 1
                    value = float(re.findall(r'[-+]?\d*\.\d+|\d+', line)[-1])
                    sequential_time_table[1][iteration] = value
            elif "Aproximacion secuencial de pi" in line:
                brackets = re.findall(r'\[(.*?)\]', line)
                if brackets:
                    iteration = int(brackets[0]) - 1
                    value = float(re.findall(r'[-+]?\d*\.\d+|\d+', line)[-1])
                    sequential_approximation_table[1][iteration] = value

    parallel_time_df = pd.DataFrame(parallel_time_table).T
    parallel_time_df.index = ['Hilo ' + str(i) for i in parallel_time_df.index]
    parallel_time_df.columns = ['Iteración ' + str(i) for i in range(1, 11)]

    speedup_df = pd.DataFrame(speedup_table).T
    speedup_df.index = ['Hilo ' + str(i) for i in speedup_df.index]
    speedup_df.columns = ['Iteración ' + str(i) for i in range(1, 11)]

    efficiency_df = pd.DataFrame(efficiency_table).T
    efficiency_df.index = ['Hilo ' + str(i) for i in efficiency_df.index]
    efficiency_df.columns = ['Iteración ' + str(i) for i in range(1, 11)]

    sequential_time_df = pd.DataFrame(sequential_time_table).T
    sequential_time_df.index = ['Hilo ' + str(i) for i in sequential_time_df.index]
    sequential_time_df.columns = ['Iteración ' + str(i) for i in range(1, 11)]

    parallel_approximation_df = pd.DataFrame(parallel_approximation_table).T
    parallel_approximation_df.index = ['Hilo ' + str(i) for i in parallel_approximation_df.index]
    parallel_approximation_df.columns = ['Iteración ' + str(i) for i in range(1, 11)]

    sequential_approximation_df = pd.DataFrame(sequential_approximation_table).T
    sequential_approximation_df.index = ['Hilo ' + str(i) for i in sequential_approximation_df.index]
    sequential_approximation_df.columns = ['Iteración ' + str(i) for i in range(1, 11)]

    with pd.ExcelWriter('output.xlsx') as writer:
        parallel_time_df.to_excel(writer, sheet_name='Tiempo Paralelo')
        speedup_df.to_excel(writer, sheet_name='Speedup')
        efficiency_df.to_excel(writer, sheet_name='Eficiencia')
        sequential_time_df.to_excel(writer, sheet_name='Tiempo Secuencial')
        parallel_approximation_df.to_excel(writer, sheet_name='Aproximación Paralela de pi')
        sequential_approximation_df.to_excel(writer, sheet_name='Aproximación Secuencial de pi')

parse_txt_to_csv('archivo.txt')
