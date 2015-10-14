/*  Copyright (C) 2009, 2010 Stewart Adam
 *  This file is part of fwbackups.
 
 * fwbackups is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * fwbackups is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with fwbackups; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <QString>
#include <QFile>
#include <QTextStream>
#include <stdio.h>
#include <time.h>

#include "common.h"
#include "config.h"
#include "logger.h"

/* TODO:
 * register() - link an output sink to log_message
 * unregister() - unlink an output sink from log_message
 */

fwLogger *fwLogger::loggerInstance = NULL;

fwLogger* fwLogger::getInstance() {
  if (loggerInstance == NULL) {
    loggerInstance = new fwLogger();
  }
    return loggerInstance;
}


void fwLogger::deleteInstance() {
  // If an instance exists, delete it
  if (loggerInstance != NULL) {
    delete loggerInstance;
    loggerInstance = NULL;
  }
}

QString fwLogger::get_log_directory() {
  /* Keeping this in a separate function gives us the option of moving the log
     somewhere else later. Until there's a need to do so, just return the
     configuration directory. */
  return get_configuration_directory();
}

QString fwLogger::get_log_location() {
  return join_path(get_log_directory(), "fwbackups-userlog.txt");
}

bool fwLogger::log_message(int message_level, QString message_text) {
  QFile file;
  QString message_prefix;
  time_t rawtime;
  struct tm * timeinfo;
  char message_time[80];
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(message_time, 80, "%b %d %Y %H:%M:%S :: ", timeinfo);
  message_prefix = message_time;
  switch(message_level) {
    case LEVEL_DEBUG:
      message_prefix += "DEBUG: ";
      break;
    case LEVEL_INFO:
      message_prefix += "INFORMATION: ";
      break;
    case LEVEL_WARNING:
      message_prefix += "WARNING: ";
      break;
    case LEVEL_ERROR:
      message_prefix += "ERROR: ";
      break;
    case LEVEL_CRITICAL:
      message_prefix += "CRITICAL: ";
      break;
  }
  // Construct the final log message
  message_text.prepend(message_prefix);
  if ( !message_text.endsWith("\n") ) {
    message_text.append("\n");
  }
  // Print it
  fprintf(stderr, message_text.toUtf8().data());
  // Log it
  file.setFileName( get_log_location() );
  file.open(QIODevice::Append | QIODevice::Text);
  // FIXME: is file.open() needed?
  QTextStream textStream(&file);
  textStream << message_text;
  file.close();
  return true;
}