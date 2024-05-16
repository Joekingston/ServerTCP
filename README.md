# TCP Logger Server

## Overview

The TCP Logger Server is a robust logging solution designed to receive logs in JSON format over a TCP connection. It supports various logging levels, application-specific log descriptions, and includes features such as rate limiting and customizable time formats.

## Features

- **JSON Log Format**: Logs must be sent in JSON format, including mandatory and optional fields.
- **Logging Levels**: Support for various logging levels (e.g., DEBUG, INFO, WARNING, ERROR, CRITICAL).
- **Rate Limiting**: Configurable rate limiting to control the number of logs received within a specified time frame and define the ban duration for exceeding the limit.
- **Time Format Customization**: Flexibility to change the time format according to your preferences.

## Log JSON Format

Logs should be sent in the following JSON format:

{
    "LEVEL": "INFO",               // Required: Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
    "application_name": "MyApp",   // Optional: Name of the application
    "log_description": "This is a log message" // Optional: Detailed description of the log
}

## Configuration

### Rate Limiting

The server supports rate limiting to control the flow of logs. You can configure:

- **Logs Per Time Frame**: Maximum number of logs allowed within a specified time frame.
- **Ban Duration**: Duration for which logging will be banned if the rate limit is exceeded.

### Time Format

Customize the time format to fit your needs. The server allows you to define the format in which timestamps are logged.

## Test Client

[Test Client Repository](https://github.com/Joekingston/A3LogClient)
