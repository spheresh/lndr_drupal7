<?php

/**
 * @file
 * Code for the Lndr debug functions.
 */

/**
 * Record and debug the incoming request.
 */
function lndr_example_service_debug() {
drush_print_r(__FUNCTION__);


  $debug = array(
    'date time' => date(('Y-m-d H:i:s')),
    'request_headers' => getallheaders(),
    'server' => $_SERVER,
    'request' => $_REQUEST,
  );

  variable_set('lndr_service_debug', $debug);
  return 'nothing to see here';
}
