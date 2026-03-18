/*  
    作者：B113040045許育菖
    C++版本：-std=c++17
    輸入：初始request所在的cylinder
    輸出：每個排程磁頭經過多少個cylinder，以及總延遲時間latency
*/

//	注意！請勿使用dec C++編譯執行，會出錯。請使用visual studio，其餘編譯器則不清楚情況。

#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <numeric>  //  to use accumulate

using namespace std;

//  the max number of cylinders
const int NUM_CYLINDERS = 5000;

//  the number of cylinder to select
const int NUM_REQUESTS = 1000;


// Function to generate random requests

vector<int> generate_requests(const int num_requests, const int num_cylinders) {
    //  the request list
    vector<int> requests(num_requests);

    //  set the cylinder in request
    cout << "[ list of cylinder ]\n";
    for (int i = 0; i < num_requests; i++) {
        requests[i] = rand() % num_cylinders;
        cout << "cylinder " << i << " " << requests[i] << '\n';
    }
    cout << '\n';
    return requests;
}

// Function to calculate the total head movement and average latency
pair<int, double> calculate_metrics(const std::vector<int>& head_movements) {
    int total_head_movement = accumulate(head_movements.begin(), head_movements.end(), 0);
    double latency = static_cast<double>(total_head_movement) / 100.0; // 1ms per 100 cylinders
    return { total_head_movement, latency };
}

/*  FCFS algorithm  */
//  按照順序進行處理
//  所以直接將requests中相鄰兩個cylinder的距離相加起來即可，記得距離要取絕對值
//      參數：起始index、requests list
//      回傳：磁碟讀寫頭移動路徑長、平均每移動到下一個cylinder的延遲
pair<int, double> fcfs(int start, const vector<int>& requests) {
    int current_position = start;

    int total_head_movement = 0;
    double latency = 0.0;
    
    //  將相鄰兩個cylinder的距離相加
    for (int request : requests) {
        total_head_movement += abs(current_position - request);
        current_position = request;
    }

    latency = static_cast<double>(total_head_movement) / 100.0;

    return { total_head_movement, latency };
}

/*  SSTF algorithm  */
//  最靠近當前位置的先處理
//  遍歷cylinder list，紀錄與當前位置最靠近的cylinder，然後將磁碟讀寫頭移動到該位置
//      參數：起始index、requests list
//      回傳：磁碟讀寫頭移動路徑長、平均每移動到下一個cylinder的延遲
pair<int, double> sstf(int start, vector<int> requests)
{
    vector<int> head_movements;
    int current_position = start;

    //  遍歷cylinder list，並紀錄與當前位置最靠近的cylinder
    while (!requests.empty())
    {
        auto closest = min_element(requests.begin(), requests.end(), [current_position](int a, int b){
            return abs(current_position - a) < abs(current_position - b);
        } );
        head_movements.push_back( abs(current_position - *closest) );
        current_position = *closest;
        requests.erase(closest);    //  從list刪除當前最靠近的cylinder，以避免重複計算
    }
    return calculate_metrics(head_movements);
}

/*  SCAN algorithm  */
//  讓磁頭不斷地在磁碟兩端進行移動，每到達一端後就將掃描方向反向
//  （但在這個project中，只要掃描一次即可，所以有沒有將掃描方向反向都沒差）
//      參數：起始index、requests list
//      回傳：磁碟讀寫頭移動路徑長、平均每移動到下一個cylinder的延遲
pair<int, double> scan(int start, vector<int> requests, bool up)
{
    //  將cylinder按照小到大排序
    sort(requests.begin(), requests.end());

    vector<int> head_movements;
    int current_position = start;
    
    if (up) //  正向
    {
        for (int request : requests)
        {
            if (request >= current_position)
            {
                head_movements.push_back(abs(current_position - request));
                current_position = request;
            }
        }
        // 讓磁頭掃描到末端 
        if (current_position != NUM_CYLINDERS - 1)
        {
            head_movements.push_back(abs(current_position - (NUM_CYLINDERS - 1)));
            current_position = NUM_CYLINDERS - 1;
        }

        for (auto it = requests.rbegin(); it != requests.rend(); ++it)
        {
            if (*it < start) // 只掃描還沒處理過的cylinder
            {
                head_movements.push_back(abs(current_position - *it));
                current_position = *it;
            }
        }
    }
    else //  反向
    {
        for (auto it = requests.rbegin(); it != requests.rend(); ++it)
        {
            if (*it <= current_position)
            {
                head_movements.push_back(abs(current_position - *it));
                current_position = *it;
            }
        }

        // 讓磁頭掃描到最前端 
        if (current_position != 0)
        {
            head_movements.push_back(abs(current_position - 0));
            current_position = 0;
        }

        for (int request : requests)
        { 
            if (request > start)
            {// 只掃描還沒處理過的cylinder
                head_movements.push_back(abs(current_position - request));
                current_position = request;
            }
        }
    }
    return calculate_metrics(head_movements);
}

/*  C-SCAN algorithm   */
//  磁頭從頭掃描到尾，掃完後再回到最前端
//      參數：起始index、requests list
//      回傳：磁碟讀寫頭移動路徑長(int)、平均每移動到下一個cylinder的延遲(double)
pair<int, double> c_scan(int start, vector<int> requests)
{
    //  將cylinder按照小到大排序
    sort(requests.begin(), requests.end());
    vector<int> head_movements;
    int current_position = start;

    for (int request : requests)
    {
        if (request >= current_position)
        {
            head_movements.push_back(abs(current_position - request));
            current_position = request;
        }
    }

    // 讓磁頭掃描到末端 
    if (current_position != NUM_CYLINDERS - 1)
    {
        head_movements.push_back(abs(current_position - (NUM_CYLINDERS - 1)));
        current_position = NUM_CYLINDERS - 1;
    }

    //  掃描到末端後，讓磁頭回到最前端
    head_movements.push_back(NUM_CYLINDERS - 1);
    current_position = 0;

    for (int request : requests)
    {
        if (request >= current_position)
        {
            head_movements.push_back(abs(current_position - request));
            current_position = request;
        }
    }
    return calculate_metrics(head_movements);
}

/*  LOOK algorithm  */
//  與SCAN類似，但只掃瞄到最遠的cylinder後就反向，而不是掃描到末端
//      參數：起始index、requests list
//      回傳：磁碟讀寫頭移動路徑長、平均每移動到下一個cylinder的延遲
pair<int, double> look(int start, vector<int> requests, bool up)
{
    //  將request按照小到大排序
    sort(requests.begin(), requests.end());
    vector<int> head_movements;
    int current_position = start;
    if (up) // 正向
    {
        for (int request : requests)
        {
            if (request >= current_position)
            {
                head_movements.push_back(abs(current_position - request));
                current_position = request;
            }
        }

        //  轉向再掃一次

        for (auto it = requests.rbegin(); it != requests.rend(); ++it)
        {
            if (*it < start) // 只掃描出始位置之前的request
            {
                head_movements.push_back(abs(current_position - *it));
                current_position = *it;
            }
        }
    }
    else // 反向
    {
        for (auto it = requests.rbegin(); it != requests.rend(); ++it)
        {
            if (*it <= current_position)
            {
                head_movements.push_back(abs(current_position - *it));
                current_position = *it;
            }
        }

        //  轉向再掃一次

        for (int request : requests)
        {
            if (request > start) // 只掃描出始位置之前的request
            {
                head_movements.push_back(abs(current_position - request));
                current_position = request;
            }
        }
    }
    return calculate_metrics(head_movements);
}

/*  C-LOOK algorithm   */
//  C-SCAN和LOOK的結合體。掃描到最遠的request後，又回到最出的請求位置。
pair<int, double> c_look(int start, vector<int> requests)
{
    //  將request按照小到大排序
    sort(requests.begin(), requests.end());
    vector<int> head_movements;
    int current_position = start;
    for (int request : requests)
    {
        if (request >= current_position)
        {
            head_movements.push_back(std::abs(current_position - request));
            current_position = request;
        }
    }

    //  再從頭開始掃
    for (int request : requests)
    {
        if (request < start)
        {
            head_movements.push_back(std::abs(current_position - request));
            current_position = request;
        }
    }
    return calculate_metrics(head_movements);
}

/*  Optimal algorithm   */ 
//  最佳解，但只限定此project的條件下
//  LOOK的改良版，由起始點位置判斷要先往前還是先往後
pair<int, double> optimal(int start, std::vector<int> requests)
{
    //  將request按照小到大排序
    sort(requests.begin(), requests.end());
    vector<int> head_movements;
    int current_position = start;

    //  先往末端掃，再往前端掃
    if (current_position >= NUM_CYLINDERS / 2)
    {
        for (int request : requests)
        {
            if (request >= current_position)
            {
                head_movements.push_back(abs(current_position - request));
                current_position = request;
            }
        }

        for (auto it = requests.rbegin(); it != requests.rend(); ++it)
        {
            if (*it < start) // 只掃描出始位置之前的request
            {
                head_movements.push_back(abs(current_position - *it));
                current_position = *it;
            }
        }
    }
    //  先往前端掃，再往末端掃
    else 
    {
        for (auto it = requests.rbegin(); it != requests.rend(); ++it)
        {
            if (*it <= current_position)
            {
                head_movements.push_back(abs(current_position - *it));
                current_position = *it;
            }
        }

        for (int request : requests)
        {
            if (request > start) // 只掃描出始位置之前的request
            {
                head_movements.push_back(abs(current_position - request));
                current_position = request;
            }
        }
    }
    return calculate_metrics(head_movements);
}

int main()
{
    //  cin, cout加速
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    cout.sync_with_stdio(false);

    //  亂樹種子
    srand(static_cast<unsigned int>(time(nullptr)));

    //  generate requests
    vector<int> requests = generate_requests(NUM_REQUESTS, NUM_CYLINDERS);
    int initial_position;

    //  每次test的延遲時間加總
    double latency[7] = { 0 };

    //  case數量
    double test_case = 0.0;

    cout << "請輸入磁頭初始位置0~" << NUM_CYLINDERS - 1 << "，輸入-1結束此程式。\n";
    cout << "initial position: ";
    while (cin >> initial_position && initial_position != -1)
    {
        if (initial_position < 0 || initial_position >= NUM_CYLINDERS)
        {
            cerr << "Out of range!\n\n";
            cout << "請輸入磁頭初始位置0~" << NUM_CYLINDERS - 1 << "，輸入-1結束此程式。\n";
            cout << "initial position: ";
            continue;
        }

        test_case += 1.0;

        auto fcfs_result = fcfs(initial_position, requests);
        auto sstf_result = sstf(initial_position, requests);
        auto scan_result = scan(initial_position, requests, true);
        auto c_scan_result = c_scan(initial_position, requests);
        auto look_result = look(initial_position, requests, true);
        auto c_look_result = c_look(initial_position, requests);
        auto optimal_result = optimal(initial_position, requests);

        latency[0] += fcfs_result.second;
        latency[1] += sstf_result.second;
        latency[2] += scan_result.second;
        latency[3] += c_scan_result.second;
        latency[4] += look_result.second;
        latency[5] += c_look_result.second;
        latency[6] += optimal_result.second;

        //  output result
        cout << "\tFCFS: Total Head Movement = "	<< fcfs_result.first	<< " cylinders, Latency = " << fcfs_result.second	<< " ms\n";
        cout << "\tSSTF: Total Head Movement = "	<< sstf_result.first	<< " cylinders, Latency = " << sstf_result.second	<< " ms\n";
        cout << "\tSCAN: Total Head Movement = "	<< scan_result.first	<< " cylinders, Latency = " << scan_result.second	<< " ms\n";
        cout << "\tC-SCAN: Total Head Movement = "	<< c_scan_result.first	<< " cylinders, Latency = " << c_scan_result.second	<< " ms\n";
        cout << "\tLOOK: Total Head Movement = "	<< look_result.first	<< " cylinders, Latency = " << look_result.second	<< " ms\n";
        cout << "\tC-LOOK: Total Head Movement = "	<< c_look_result.first	<< " cylinders, Latency = " << c_look_result.second	<< " ms\n";
        cout << "\tOptimal: Total Head Movement = " << optimal_result.first << " cylinders, Latency = " << optimal_result.second<< " ms\n";

        cout << "\n請輸入磁頭初始位置0 ~ " << NUM_CYLINDERS - 1 << "，輸入-1結束此程式。\n";
        cout << "initial position: ";
    }

    //  final output
    cout << "\n平均延遲時間\n";
    cout << "FCFS: Latency = " << latency[0] / test_case << "ms\n";
    cout << "SSTF: Latency = " << latency[1] / test_case << "ms\n";
    cout << "SCAN: Latency = " << latency[2] / test_case << "ms\n";
    cout << "C-SCAN: Latency = " << latency[3] / test_case << "ms\n";
    cout << "LOOK: Latency = " << latency[4] / test_case << "ms\n";
    cout << "C-LOOK: Latency = " << latency[5] / test_case << "ms\n";
    cout << "Optimal: Latency = " << latency[6] / test_case << "ms\n";

    return 0;
}
