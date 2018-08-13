<?php

/**
 * @file
 * Code for the Lndr module.
 */

/**
 * An example function for users to test how lander page will behavior
 * published to Drupal.
 *
 * @param $form
 * @param $form_state
 *
 * @return array
 */
function lndr_example($form, &$form_state) {
  $form['lndr_example_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Test landing page URL'),
    '#description' => t("Enter the internal URL of a Lndr landing page here"),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Test'),
  );

  return $form;
}

/**
 * Validation handler.
 *
 * @param $form
 * @param $form_state
 */
function lndr_example_validate($form, &$form_state) {
  if (!valid_url($form_state['values']['lndr_example_url'], TRUE)) {
    form_set_error('lndr_example_url', t('Please enter a valid URL'));
  }
  $internal_url = drupal_parse_url($form_state['values']['lndr_example_url']);
  if (substr($internal_url['path'], 0, strlen(LNDR_BASE_PUBLISHED)) != LNDR_BASE_PUBLISHED) {
    form_set_error('lndr_example_url', t('only lndr specific URLs can be used, it should start with @url', array('@url' => LNDR_BASE_PUBLISHED)));
  }
}

/**
 * Submission handler.
 *
 * @param $form
 * @param $form_state
 */
function lndr_example_submit($form, &$form_state) {
  drupal_goto('examples/lndr/render', array('query' => array('url' => $form_state['values']['lndr_example_url'])));
}

/**
 * Simple page to show how lndr landing page looks in Drupal.
 *
 * @return string
 */
function lndr_example_render() {
  $query = drupal_get_query_parameters();
  if (!isset($query['url'])) {
    return 'Invalid url passed';
  }
  // Add trailing slash.
  if (substr($query['url'], -1, 1) !== '/') {
    $query['url'] .= '/';
  }
  print _lndr_import_page($query['url']);
}

/**
 * Emulate the lndr web service call.
 */
function lndr_example_service() {
  $path = drupal_get_path('module', 'lndr_example') . '/lndr_test.json';
  $data = file_get_contents($path);
  print $data;
}
