/*!
 * @file    common.h
 * @author  Tyler Holmes
 * @date    2-Sept-2017
 * @brief   Common functions and structures for all files.
 */

#pragma once



//! Common return type for the entire project
typedef enum {
    RET_OK,         //!< The return code if all goes well
    RET_VAL_ERR,    //!< If the values given cause an error
    RET_NODATA_ERR, //!< If not enough data was given
    RET_NOMEM_ERR,  //!< If not enough memory is available
    RET_LEN_ERR,    //!< If there was some sort of length related error
    RET_COM_ERR,    //!< Some sort of communication error
    RET_BUSY_ERR,   //!< Some sort of busy resource / timeout
    RET_GEN_ERR,    //!< Lame catch all general error
    RET_NORPT_ERR,  //!< No available report
    RET_INVALID_ARGS_ERR,  //!< Incorrect arguments to the function called
    RET_MAX_LEN_ERR,       //!< Maximum length was violated
    RET_WDG_SET,           //!< Return code if the watchdog flag was set on reset
    RET_CAL_ERR,           //!< General error relating to calibration
    RET_BAD_CHECKSUM,      //!< General checksum related error (didn't match)
} ret_t;


void _Error_Handler(char *, int);

void Error_Handler_with_retval(char * file, int line, int retval);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
