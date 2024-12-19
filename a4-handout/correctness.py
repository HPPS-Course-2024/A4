import subprocess
#We have tested naive, by essentially looking in the tsv file and manually by eye checking if the output is correct, so we know the naive implementation is correct
# ./id_query_naive    data/20000_samples.tsv < test_input.txt > expected_output.txt

# ./id_query_indexed  data/20000_samples.tsv < test_input.txt > actual_output.txt
# compare expected_output.txt actual_output
# rm expected_output.txt
# ./id_query_binsort  data/20000_samples.tsv < test_input.txt 
# compare expected_output.txt actual_output
# rm expected_output.txt
SAMPLES = 2000
QUERY_FILE = "data/20000_samples.tsv"
INPUT_FILE_PATH = F"benchmark/{SAMPLES}_input.txt" 
EXPECTED_FILE_PATH = "benchmark/expected_output.txt"
ACTUAL_INDEXED_FILE_PATH = "benchmark/actual_output_indexed.txt"
ACTUAL_BINSORT_FILE_PATH = "benchmark/actual_output_binsort.txt"
MAKE_CMD = "make"

GENERATE_DATA = (
    f"./random_ids {QUERY_FILE} | head -n {SAMPLES} > {INPUT_FILE_PATH}"
)

ID_QUERY_NAIVE_CMD = (
  f"./id_query_naive {QUERY_FILE} < {INPUT_FILE_PATH} > {EXPECTED_FILE_PATH}"
)

ID_QUERY_INDEXED_CMD = (
  f"./id_query_indexed {QUERY_FILE} < {INPUT_FILE_PATH} > {ACTUAL_INDEXED_FILE_PATH}"
)

ID_QUERY_BINSORT_CMD = (
  f"./id_query_binsort  {QUERY_FILE} < {INPUT_FILE_PATH} > {ACTUAL_BINSORT_FILE_PATH}"
)

CLEAN_INPUT_DATA_CMD = f"rm {INPUT_FILE_PATH}"
CLEAN_EXPECTED_CMD = f"rm {EXPECTED_FILE_PATH}"
CLEAN_ACTUAL_INDEXED_CMD = f"rm {ACTUAL_INDEXED_FILE_PATH}"
CLEAN_ACTUAL_BINSORT_CMD = f"rm {ACTUAL_BINSORT_FILE_PATH}"

def run_command(command):
    try:
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error while running command: {e}")
        raise

FILTER_WORDS = ['Reading', 'Building', 'Query time:', 'Total query runtime:']
def compare_files(file1, file2):
    try: 
        with open(file1, 'r') as f1, open(file2, 'r') as f2:
            #skip words
            f1_lines = filter(lambda word: word not in FILTER_WORDS, f1.readlines())
            f2_lines = filter(lambda word: word not in FILTER_WORDS, f2.readlines())
            result = f1.readlines() == f2.readlines()
            print(f"Comparing {file1} and {file2}: {result}")
            return result
    except Exception as e: 
        print(f"Error while comparing files: {e}")
        raise

try:
    run_command(MAKE_CMD)
    run_command(GENERATE_DATA)

    # Actual data
    run_command(ID_QUERY_NAIVE_CMD)
    
    # Indexed query
    run_command(ID_QUERY_INDEXED_CMD)
    compare_files(EXPECTED_FILE_PATH, ACTUAL_INDEXED_FILE_PATH)
    run_command(CLEAN_ACTUAL_INDEXED_CMD)
    
    # Binsort query
    run_command(ID_QUERY_BINSORT_CMD)
    compare_files(EXPECTED_FILE_PATH, ACTUAL_BINSORT_FILE_PATH)
    run_command(CLEAN_ACTUAL_BINSORT_CMD)
    
    # Clean expected/actual output and the generated data
    run_command(CLEAN_EXPECTED_CMD)
    run_command(CLEAN_INPUT_DATA_CMD)
except Exception as e:
    print(f"Error: {e}")
    raise