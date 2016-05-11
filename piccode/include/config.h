/* 
 * File:   config.h
 * Author: David
 *
 * Created on 10 de mayo de 2016, 7:00
 */

#ifndef CONFIG_H
#define	CONFIG_H

struct Config {
    int16 pulse_time;                   //In us
    int8 start_sample_delay;            //In us after coil pulse ends
    int8 sample_time;                   //In us
    int8 start_second_sample_delay;     //In us
};

extern struct Config config;

void cnf_load();
void cnf_save_coil_pulse();
void cnf_save_start_sample_delay();
void cnf_save_sample_time();
void cnf_save_start_second_sample_delay();

#endif	/* CONFIG_H */

