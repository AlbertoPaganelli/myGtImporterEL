/***************************************************************************
                          main.cpp
                             -------------------
    copyright            : (C) 2013 Montanari, Paganelli, Bulgarelli
    email                : bulgarelli@iasfbo.inaf.it, 61825@studenti.unimore.it,

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <iomanip>
#include <string>
#include "InputFileFITS.h"

#include <Freeze/Freeze.h>
#include "AstroMap.h"

//file types
#define EVT 1
#define LOG 2

//columns of EVT file
#define EVT_TIME 0
#define EVT_ENERGY 5
#define EVT_PH_EARTH 14
#define EVT_THETA 1
#define EVT_PHASE 15
#define EVT_EVSTATUS 14

//columsn of LOG file
#define LOG_TIME 0
#define LOG_PHASE 5
#define LOG_LIVETIME 39
#define LOG_LOG_STATUS 40
#define LOG_MODE 4
#define LOG_ATTITUDE_RA_Y 13
#define LOG_ATTITUDE_DEC_Y 14
#define LOG_EARTH_RA 24
#define LOG_EARTH_DEC 25
#define LOG_Q1 9
#define LOG_Q2 10
#define LOG_Q3 11
#define LOG_Q4 12

using namespace std;
using namespace qlbase;

///The Ice communicator;
Ice::CommunicatorPtr communicator;

///The Freeze connection
Freeze::ConnectionPtr connection;

/// Write the data on BDB.
/// \param[in] filename The name of the file that contains the data.
/// \param[in] nrow The limit of row to be read (optional.
/// \return An integer that could be used for error control
int writeOnBDB(string filename, int nrow) {

	InputFileFITS* inputFF;


		//open input file FITS
		try{
			inputFF = new InputFileFITS();
			inputFF->open(filename);
			inputFF->moveToHeader(1); //CHECK base index AZ


			//check input file fits type: EVT or LOG
			int ncols = inputFF->getNCols();
			int nrows_start = 0;
			int nrows_end = inputFF->getNRows();
			if(nrow != -1) {
				nrows_end = nrow;
			}
			int type;

			if(ncols == 19) type = EVT;
			if(ncols == 41) type = LOG;
			Ice::InitializationData initData;
			initData.properties = Ice::createProperties();
			initData.properties->load("config");

			if(type == EVT) {
				//read all columns
				cout << "Read EVT file " << endl;
				std::vector<double> time = inputFF->read64f(EVT_TIME, nrows_start, nrows_end-1);
				std::vector<float> energy = inputFF->read32f(EVT_ENERGY, nrows_start, nrows_end-1);
				std::vector<float> ph_earth = inputFF->read32f(EVT_PH_EARTH, nrows_start, nrows_end-1);
				std::vector<float> theta = inputFF->read32f(EVT_THETA, nrows_start, nrows_end-1);
				std::vector<int16_t> phase = inputFF->read16i(EVT_PHASE, nrows_start, nrows_end-1);

				//Create the vector to store into BDB
				Astro::agileEvt evt;

				//Create the map
				AgileEvtMap evtMap(connection, "AgileEvtMap");

				//write data into BDB
				for(uint32_t i  = 0; i<nrows_end; i++) {
					//cout << setiosflags(ios::fixed) << std::setprecision(6) << (double) time[i] << "\t" << energy[i] << "\t" << ph_earth[i]  << "\t" << theta[i]  << "\t" << phase[i] << endl;

					//Populate the vector
					evt.clear();
					evt.push_back((Ice::Double) phase[i]);
					evt.push_back((Ice::Double) theta[i]);
					evt.push_back((Ice::Double) ph_earth[i]);
					evt.push_back((Ice::Double) energy[i]);
					evt.push_back((Ice::Double) time[i]);

					//Execute write
					evtMap.insert(make_pair(time[i],evt));
				}
			}
			if(type == LOG) {
				//read all columns
				cout << "Read LOG file " << nrows_end << endl;
				std::vector<double> time = inputFF->read64f(LOG_TIME, nrows_start, nrows_end-1);
				std::vector<int16_t> phase = inputFF->read16i(LOG_PHASE, nrows_start, nrows_end-1);
				std::vector<float> livetime = inputFF->read32f(LOG_LIVETIME, nrows_start, nrows_end-1);
				std::vector<int16_t> log_status = inputFF->read16i(LOG_LOG_STATUS, nrows_start, nrows_end-1);
				std::vector<int16_t> mode = inputFF->read16i(LOG_MODE, nrows_start, nrows_end-1);
				std::vector<double> attitude_ra_y = inputFF->read64f(LOG_ATTITUDE_RA_Y, nrows_start, nrows_end-1);
				std::vector<double> attitude_dec_y = inputFF->read64f(LOG_ATTITUDE_RA_Y, nrows_start, nrows_end-1);
				std::vector<double> log_earth_ra = inputFF->read64f(LOG_EARTH_RA, nrows_start, nrows_end-1);
				std::vector<double> log_earth_dec = inputFF->read64f(LOG_EARTH_DEC, nrows_start, nrows_end-1);
				std::vector<float> q1 = inputFF->read32f(LOG_Q1, nrows_start, nrows_end-1);
				std::vector<float> q2 = inputFF->read32f(LOG_Q2, nrows_start, nrows_end-1);
				std::vector<float> q3 = inputFF->read32f(LOG_Q3, nrows_start, nrows_end-1);
				std::vector<float> q4 = inputFF->read32f(LOG_Q4, nrows_start, nrows_end-1);

				//Create the vector to store into BDB
				Astro::agileLog agileLog;

				//Create the map
				AgileLogMap logMap(connection, "AgileLogMap");

				//write data into BDB
				uint32_t count = 0;
				for(uint32_t i = 0; i<nrows_end; i++) {
	//				cout << setiosflags(ios::fixed) << std::setprecision(6) << (double) time[i] << " ";
	//				cout << attitude_ra_y[i] << endl;

					//Populate the vector
					agileLog.clear();
					agileLog.push_back((Ice::Double) q4[i]);
					agileLog.push_back((Ice::Double) q3[i]);
					agileLog.push_back((Ice::Double) q2[i]);
					agileLog.push_back((Ice::Double) q1[i]);
					agileLog.push_back((Ice::Double) log_earth_dec[i]);
					agileLog.push_back((Ice::Double) log_earth_ra[i]);
					agileLog.push_back((Ice::Double) attitude_dec_y[i]);
					agileLog.push_back((Ice::Double) attitude_ra_y[i]);
					agileLog.push_back((Ice::Double) mode[i]);
					agileLog.push_back((Ice::Double) log_status[i]);
					agileLog.push_back((Ice::Double) livetime[i]);
					agileLog.push_back((Ice::Double) phase[i]);
					agileLog.push_back((Ice::Double) time[i]);

					//Execute write
					logMap.insert(make_pair(time[i], agileLog));

					if(attitude_ra_y[i] != 0) count++;
				}
				cout << count << endl;
			}

			return 0;

		} catch(IOException* e) {
			cout << e->getErrorCode() << ": " << e->what() << endl;
			return e->getErrorCode();
		}

}

/// Read the agileEvt data from BDB
void readEvt(){
	//The map
	AgileEvtMap evtMap(connection,"AgileEvtMap");

	//The iterator
	AgileEvtMap::iterator it;

	//The evt vector
	Astro::agileEvt evt;

	//Read the data
	for (it = evtMap.begin(); it != evtMap.end(); ++it) {
		evt = it->second;
		for (int var = 0; var < Astro::agileEvtSize; ++var) {
			if(var == 0)
				cout << setiosflags(ios::fixed) << std::setprecision(6) << (double) evt.back();
			else
				cout << evt.back() << "\t";
			evt.pop_back();
		}
		cout << endl;
	}
}

/// Read the agileLog data from BDB
void readLog() {
	//The map
	AgileLogMap logMap(connection,"AgileLogMap");

	//The iterator
	AgileLogMap::iterator it;

	//The evt vector
	Astro::agileLog agileLog;

	//Read the data
	for (it = logMap.begin(); it != logMap.end(); ++it) {
		agileLog = it->second;
		for (int var = 0; var < Astro::agileLogSize; ++var) {
			if (var == 0)
				cout << setiosflags(ios::fixed) << std::setprecision(6) << agileLog.back();
			else
				cout << agileLog.back() << "\t";
			agileLog.pop_back();
		}
		cout << endl;
	}
}

///Import AGILE LOG and EVT files into Ice/Freeze/BDB
int main(int argc, char** argv) {

	//Initialize Ice
	Ice::InitializationData initData;
	initData.properties = Ice::createProperties();
	initData.properties->load("config");

	// Initialize the Communicator.
	communicator = Ice::initialize(argc, argv, initData);

	// Create a Freeze database connection.
	connection = Freeze::createConnection(communicator, "db");

	//Read the mode
	cout << "Select mode: 0 to read or 1 write: ";
	string s;
	cin >> s;
	int mode = atoi(s.c_str());

	if (mode == 0 ) { //READ
		cout << "Read: 0 Evt or 1 Log: ";
		cin >> s;
		int type = atoi(s.c_str());
		if (type == 0) {
			readEvt();
		} else {
			readLog();
		}
	} else { //WRITE
		cout << "Insert filename: ";
		string filename;
		cin >> filename;

		cout << "Insert the last line to be read (optional, 0 to read all): ";
		cin >> s;
		int nrow = atoi(s.c_str());
		if(nrow==0)
			nrow = -1;

		writeOnBDB(filename, nrow);
	}

	return 0;

}
