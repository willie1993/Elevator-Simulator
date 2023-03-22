#include <iostream>
#include <vector>
#include <queue>
#include <thread>

#define ELEVATOR_NUMS 2

using namespace std;

class Elevator {
public:
    Elevator(int input_id);
    void display_floor();
    int move(int current, int floor);
    int current_floor = 1;
    bool is_moving = false;
    void working();

private:
    int id;
    int move_one_step();
    queue<int> dest_floor_queue;
};
Elevator::Elevator(int input_id) {
    id = input_id;
}

void Elevator::display_floor() {
    cout << "Elevator_" << id << ": " << current_floor << endl;
}

int Elevator::move(int current, int floor) {
    if (is_moving == true)
        return 1;
    is_moving = true;
    dest_floor_queue.push(current);
    dest_floor_queue.push(floor);
    return 0;
}

// 0 no need to move, 1 up, 2 down
int Elevator::move_one_step() {
    int dest_floor = 0;

    while (!dest_floor_queue.empty() && dest_floor_queue.front() == current_floor)
        dest_floor_queue.pop();

    if (dest_floor_queue.empty())
    {
        is_moving = false;
        return 0;
    }
    
    dest_floor = dest_floor_queue.front();

    if (dest_floor > current_floor)
    {
        current_floor++;
        return 1;
    }
    else if (dest_floor < current_floor)
    {
        current_floor--;
        return 2;
    }
    return 0;
}

void Elevator::working() {
    display_floor();
    while (1)
    {
        move_one_step();
        if (is_moving == true)
            display_floor();
        this_thread::sleep_for(chrono::seconds(1));
    }
}

int get_best_elevator(int dest_floor, vector<Elevator> &elevator_vec) {
    int best_elevator = -1;
    int best_elevator_floor = abs(elevator_vec[0].current_floor - dest_floor);
    for (int i = 0; i < elevator_vec.size(); i++)
    {
        if (elevator_vec[i].is_moving == false && abs(elevator_vec[i].current_floor - dest_floor) <= best_elevator_floor)
            best_elevator = i;
    }
    return best_elevator;
}

void elevator_control_do_task(vector<Elevator> &elevator_vec, queue<pair<int, int>> &elevator_task_vec) {
    pair<int, int> pair_current_dest;
    int task_elevator = -1;
    while (1)
    {
        if (elevator_task_vec.empty())
            continue;

        pair_current_dest = elevator_task_vec.front();

        task_elevator = get_best_elevator(pair_current_dest.first, elevator_vec);
        if (task_elevator == -1)
            continue;

        elevator_vec[task_elevator].move(pair_current_dest.first, pair_current_dest.second);
        elevator_task_vec.pop();
        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main()
{    
    vector<Elevator> elevator_vec;
    queue<pair<int, int>> elevator_task_vec;
    thread elevator_working_thread[ELEVATOR_NUMS];

    for (int i = 0; i < ELEVATOR_NUMS; i++)
    {
        elevator_vec.push_back(Elevator(i));
    }

    for (int i = 0; i < ELEVATOR_NUMS; i++)
    {
        elevator_working_thread[i] = thread(&Elevator::working, &elevator_vec[i]);
    }
    thread task_thread(elevator_control_do_task, ref(elevator_vec), ref(elevator_task_vec));

    int task_current_floor, task_dest_floor;
    while (1) {
        cin >> task_current_floor >> task_dest_floor;
        cout << "*****" << endl << "current_floor: " << task_current_floor << ", dest_floor: " << task_dest_floor << endl << "*****" << endl;
        elevator_task_vec.push(make_pair(task_current_floor, task_dest_floor));
    }

    task_thread.join();
    for (int i = 0; i < ELEVATOR_NUMS; i++)
    {
        elevator_working_thread[i].join();
    }
    return 0;
}
