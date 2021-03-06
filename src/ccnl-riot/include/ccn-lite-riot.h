/*
 * Copyright (C) 2015, 2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CCN_LITE_RIOT_H
#define CCN_LITE_RIOT_H

/**
 * @defgroup    pkg_ccnlite CCN-Lite stack
 * @ingroup     pkg
 * @ingroup     net
 * @brief       Provides a NDN implementation
 *
 * This package provides the CCN-Lite stack as a port of NDN for RIOT.
 *
 * @{
 */

#include <unistd.h>
#include "kernel_types.h"
#include "arpa/inet.h"
#include "net/packet.h"
#include "net/ethernet/hdr.h"
#include "sys/socket.h"
#include "ccnl-core.h"
#include "ccnl-pkt-ndntlv.h"
#include "net/gnrc/netreg.h"
#include "ccnl-dispatch.h"
//#include "ccnl-pkt-builder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Dynamic memory allocation used in CCN-Lite
 *
 * @{
 */
#define ccnl_malloc(s)                  malloc(s)
#define ccnl_calloc(n,s)                calloc(n,s)
#define ccnl_realloc(p,s)               realloc(p,s)
#define ccnl_free(p)                    free(p)
/**
 * @}
 */

/**
 * Constant string
 */
#define CONSTSTR(s)                     s

/**
 * Stack size for CCN-Lite event loop
 */
#define CCNL_STACK_SIZE (THREAD_STACKSIZE_MAIN)

/**
 * Size of the message queue of CCN-Lite's event loop
 */
#define CCNL_QUEUE_SIZE     (8)

/**
 * @brief Data structure for interest packet
 */
typedef struct {
    struct ccnl_prefix_s *prefix;   /**< requested prefix */
    unsigned char *buf;             /**< buffer to store the interest packet */
    size_t buflen;                  /**< size of the buffer */
} ccnl_interest_t;


/**
 * Maximum string length for prefix representation
 */
#define CCNL_PREFIX_BUFSIZE     (50)

/**
 * Message type for signalling a timeout while waiting for a content chunk
 */
#define CCNL_MSG_TIMEOUT        (0x1701)

/**
 * Message type for advancing the ageing timer
 */
#define CCNL_MSG_AGEING         (0x1702)

/**
 * Maximum number of elements that can be cached
 */
#ifndef CCNL_CACHE_SIZE
#define CCNL_CACHE_SIZE     (5)
#endif
#ifdef DOXYGEN
#define CCNL_CACHE_SIZE
#endif

/**
 * Struct holding CCN-Lite's central relay information
 */
extern struct ccnl_relay_s ccnl_relay;

/**
 * @brief Function pointer type for local producer function
 */
typedef int (*ccnl_producer_func)(struct ccnl_relay_s *relay,
                                  struct ccnl_face_s *from,
                                  struct ccnl_pkt_s *pkt);

/**
 * @brief Function pointer type for caching strategy function
 */
typedef int (*ccnl_cache_strategy_func)(struct ccnl_relay_s *relay,
                                        struct ccnl_content_s *c);

/**
 * @brief   Start the main CCN-Lite event-loop
 *
 * @return  The PID of the event-loop's thread
 */
kernel_pid_t ccnl_start(void);

/**
 * @brief Opens a @ref net_gnrc_netif device for use with CCN-Lite
 *
 * @param[in] if_pid        The pid of the @ref net_gnrc_netif device driver
 * @param[in] netreg_type   The @ref net_gnrc_nettype @p if_pid should be
 *                          configured to use
 *
 * @return 0 on success,
 * @return -EINVAL if eventloop could not be registered for @p netreg_type
 */
int ccnl_open_netif(kernel_pid_t if_pid, gnrc_nettype_t netreg_type);

/**
 * @brief Sends out an Interest
 *
 * @param[in] prefix    The name that is requested
 * @param[out] buf      Buffer to write the content chunk to
 * @param[in] buf_len   Size of @p buf
 *
 * @return 0 on success
 * @return -1 on failure
 */
int ccnl_send_interest(struct ccnl_prefix_s *prefix,
                       unsigned char *buf, size_t buf_len);

/**
 * @brief Wait for incoming content chunk
 *
 * @pre The thread has to register for CCNL_CONT_CHUNK in @ref net_gnrc_netreg
 *      first
 *
 * @post The thread should unregister from @ref net_gnrc_netreg after this
 *       function returns
 *
 * @param[out] buf      Buffer to stores the received content
 * @param[in]  buf_len  Size of @p buf
 * @param[in]  timeout  Maximum to wait for the chunk, set to a default value if 0
 *
 * @return 0 if a content was received
 * @return -ETIMEDOUT if no chunk was received until timeout
 */
int ccnl_wait_for_chunk(void *buf, size_t buf_len, uint64_t timeout);

/**
 * @brief Set a local producer function
 *
 * Setting a local producer function allows to generate content on the fly or
 * react otherwise on any kind of incoming interest.
 *
 * @param[in] func  The function to be called first for any incoming interest
 */
void ccnl_set_local_producer(ccnl_producer_func func);

/**
 * @brief Set a function to control the caching strategy
 *
 * The given function will be called if the cache is full and a new content
 * chunk arrives. It shall remove (at least) one entry from the cache.
 *
 * If the return value of @p func is 0, the default caching strategy will be
 * applied by the CCN-lite stack. If the return value is 1, it is assumed that
 * (at least) one entry has been removed from the cache.
 *
 * @param[in] func  The function to be called for an incoming content chunk if
 *                  the cache is full.
 */
void ccnl_set_cache_strategy_remove(ccnl_cache_strategy_func func);

#ifdef __cplusplus
}
#endif
#endif /* CCN_LITE_RIOT_H */
/** @} */
