#include <iostream>
#include "td3.hpp"
#include "support.hpp"
#include <stdlib.h>
#include <math.h>       // sin, cos
#include <assert.h>

using namespace std;

using namespace support;

double* extend_array(double* array, int length, int new_size) {
    // IMPLEMENT YOUR FUNCTION HERE
    double *n_array;
    n_array = new double[new_size];

    for (int i = 0; i < new_size; i++) {
        if (i < length) {
          n_array[i] = array[i];
        } else {
          n_array[i] = 0;
        }
    }
    delete array;
    return n_array;
}

double* shrink_array(double* array, int length, int new_size) {
    double *n_array;
    n_array = new double[new_size];
    for (int i = 0; i < new_size; i++) {
        n_array[i] = array[i];
    }
    delete array;
    return n_array;
}

double* append_to_array(double element,
                        double* array,
                        int &current_size,
                        int &max_size) {
  // IMPLEMENT YOUR FUNCTION HERE

    if (current_size == max_size) {
        array = extend_array(array, max_size, max_size + 5);
        max_size += 5;
    }

    array[current_size] = element;
    current_size += 1;

    return array;
}

double* remove_from_array(double* array,
                          int &current_size,
                          int &max_size) {

    array[current_size] = 0;
    current_size--;

    if (max_size - current_size >= 5) {
        array = shrink_array(array, max_size, max_size - 5);
        max_size -= 5;
    }
    return array;
}

bool simulate_projectile(const double magnitude, const double angle,
                         const double simulation_interval,
                         double *targets, int &tot_targets,
                         int *obstacles, int tot_obstacles,
                         double* &telemetry,
                         int &telemetry_current_size,
                         int &telemetry_max_size) {
  // YOU CAN MODIFY THIS FUNCTION TO RECORD THE TELEMETRY

  bool hit_target, hit_obstacle;
  double v0_x, v0_y, x, y, t;
  double PI = 3.14159265;
  double g = 9.8;

  v0_x = magnitude * cos(angle * PI / 180);
  v0_y = magnitude * sin(angle * PI / 180);

  t = 0;
  x = 0;
  y = 0;

  hit_target = false;
  hit_obstacle = false;
  while (y >= 0 && (! hit_target) && (! hit_obstacle)) {
    double * target_coordinates = find_collision(x, y, targets, tot_targets);
    if (target_coordinates != NULL) {
      remove_target(targets, tot_targets, target_coordinates);
      hit_target = true;
    } else if (find_collision(x, y, obstacles, tot_obstacles) != NULL) {
      hit_obstacle = true;
    } else {
      t = t + simulation_interval;
      y = v0_y * t  - 0.5 * g * t * t;
      x = v0_x * t;
    }
    telemetry = append_to_array(t, telemetry, telemetry_current_size, telemetry_max_size);
    telemetry = append_to_array(x, telemetry, telemetry_current_size, telemetry_max_size);
    telemetry = append_to_array(y, telemetry, telemetry_current_size, telemetry_max_size);
  }

  return hit_target;
}

void merge_telemetry(double **telemetries,
                     int tot_telemetries,
                     int *telemetries_sizes,
                     double* &global_telemetry,
                     int &global_telemetry_current_size,
                     int &global_telemetry_max_size) {

    //create an array with the current position we are looking at
    int p_telemetries[tot_telemetries];
    //total number of elements which will be in global telemetry
    int total_size = 0;
    for (int elt = 0; elt < tot_telemetries; elt++) {
        p_telemetries[elt] = 0;
        total_size += telemetries_sizes[elt];
    }


    //find the minimum time
    for (int i = 0; i < total_size; i+=3) {
        bool given = false;
        int min;

        for (int elt = 0; elt < tot_telemetries; elt++) {
            if (p_telemetries[elt] < telemetries_sizes[elt]) {
                if (!given) {
                    min = elt;
                    given = true;
                }
                else {
                    if (telemetries[elt][p_telemetries[elt]] < telemetries[min][p_telemetries[min]]) {
                        min = elt;
                    }
                }
            }
        }
        //add the time, x, y to global telemetry
        global_telemetry = append_to_array(telemetries[min][p_telemetries[min]], global_telemetry, global_telemetry_current_size, global_telemetry_max_size);
        global_telemetry = append_to_array(telemetries[min][p_telemetries[min] + 1], global_telemetry, global_telemetry_current_size, global_telemetry_max_size);
        global_telemetry = append_to_array(telemetries[min][p_telemetries[min] + 2], global_telemetry, global_telemetry_current_size, global_telemetry_max_size);


        p_telemetries[min] += 3;
    }


}






