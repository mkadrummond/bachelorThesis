/*
 * TI eQEP driver interface API
 *
 * Copyright (C) 2013 Nathaniel R. Lewis - http://nathanielrlewis.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

// Pull in our eQEP driver definitions
#include "eqep.h"

// Language dependencies
#include <cstdint>
#include <cstdlib>
#include <cstdio>

// POSIX dependencies
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>

// Constructor for eQEP driver interface object
eQEP::eQEP(std::string _path, eQEP::eQEP_Mode _mode)
    : path(_path)
{
    // Set the mode of the hardware
    this->set_eqep_mode(_mode);

    // Reset the position
    this->set_position(0);
}

// Set the position of the eQEP hardware
void eQEP::set_position(int32_t position)
{
    // Open the file representing the position
    FILE *fp = fopen((this->path + "/position").c_str(), "w");

    // Check that we opened the file correctly
    if(fp == NULL)
    {
        // Error, break out
        std::cerr << "[eQEP " << this->path << "] Unable to open position for write" << std::endl;
        return;
    }

    // Write the desired value to the file
    fprintf(fp, "%d\n", position);

    // Commit changes
    fclose(fp);
}

// Set the period of the eQEP hardware
void eQEP::set_period(long long unsigned int period)
{
    // Open the file representing the position
    FILE *fp = fopen((this->path + "/period").c_str(), "w");

    // Check that we opened the file correctly
    if(fp == NULL)
    {
        // Error, break out
        std::cerr << "[eQEP " << this->path << "] Unable to open period for write" << std::endl;
        return;
    }

    // Write the desired value to the file
    fprintf(fp, "%llu\n", period);

    // Commit changes
    fclose(fp);
}

// Set the mode of the eQEP hardware
void eQEP::set_eqep_mode(eQEP::eQEP_Mode _mode)
{
    // Open the file representing the position
    FILE *fp = fopen((this->path + "/mode").c_str(), "w");

    // Check that we opened the file correctly
    if(fp == NULL)
    {
        // Error, break out
        std::cerr << "[eQEP " << this->path << "] Unable to open mode for write" << std::endl;
        return;
    }

    // Write the desired value to the file
    fprintf(fp, "%u\n", _mode);

    // Commit changes
    fclose(fp);
}

// Get the position of the hardware
int eQEP::get_position(bool _poll)
{
    // Position temporary variable
    int      position;
    char     	dummy;
    struct pollfd ufd;

    // Do we want to poll?
    if(_poll)
    {
        // Open a connection to the attribute file.
        if ((ufd.fd = open((this->path + "/position").c_str(), O_RDWR)) < 0)
        {
            // Error, break out
            std::cerr << "[eQEP " << this->path << "] Unable to open position for polling" << std::endl;
            return 0 ;
        }

        // Dummy read
        read(ufd.fd, &dummy, 1);

        // Poll the port
        ufd.events = POLLPRI;
        if (poll(&ufd, 1, -1) < 0)
        {
            // Error, break out
            std::cerr << "[eQEP " << this->path << "] Error occurred whilst polling" << std::endl;
            close(ufd.fd);
            return 0 ;
        }
    }

    // Read the position
    FILE *fp = fopen((this->path + "/position").c_str(), "r");

    // Check that we opened the file correctly
    if(fp == NULL)
    {
        // Error, break out
        std::cerr << "[eQEP " << this->path << "] Unable to open position for read" << std::endl;
        close(ufd.fd);
        return 0 ;
    }

    // Write the desired value to the file
    fscanf(fp, "%d", &position);

    // Commit changes
    fclose(fp);

    // If we were polling, close the polling file
    if(_poll)
    {
        close(ufd.fd);
    }

    // Return the position
    return position;
}

// Get the period of the eQEP hardware
unsigned int eQEP::get_period()
{
    // Open the file representing the position
    FILE *fp = fopen((this->path + "/period").c_str(), "r");

    // Check that we opened the file correctly
    if(fp == NULL)
    {
        // Error, break out
        std::cerr << "[eQEP " << this->path << "] Unable to open period for read" << std::endl;
        return 0 ;
    }

    // Write the desired value to the file
    long long unsigned int period = 0;
    fscanf(fp, "%llu", &period);

    // Commit changes
    fclose(fp);

    // Return the period
    return period;
}

// Get the mode of the eQEP hardware
eQEP::eQEP_Mode eQEP::get_mode()
{
    // Open the file representing the position
    FILE *fp = fopen((this->path + "/mode").c_str(), "r");

    // Check that we opened the file correctly
    if(fp == NULL)
    {
        // Error, break out
        std::cerr << "[eQEP " << this->path << "] Unable to open mode for read" << std::endl;
        return eQEP::eQEP_Mode_Error;
    }

    // Write the desired value to the file
    eQEP::eQEP_Mode mode;
    fscanf(fp, "%u", (unsigned int*)&mode);

    // Commit changes
    fclose(fp);

    // Return the mode
    return mode;
}

