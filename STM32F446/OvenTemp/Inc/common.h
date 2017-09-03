/*!
 * @file    common.h
 * @author  Tyler Holmes
 * @date    2-Sept-2017
 * @brief   Common functions and structures for all files.
 */

#pragma once


void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
