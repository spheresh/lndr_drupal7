<?php
/**
 * @file
 * Code for the Lndr module
 */



/**
 * Form builder callback
 */
function lndr_admin_settings($form, &$form_state) {

  $form['lndr_token'] = array(
    '#type' => 'textfield',
    '#title' => t('API token'),
    '#default_value' => variable_get('lndr_token', ''),
    '#description'   => t("Your Lndr account API token, you can find this information under your user profile in Lndr"),
  );

  $form['lndr_debug_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Developer mode'),
    '#default_value' => variable_get('lndr_debug_mode', 0),
    '#description' => t('Turn this on to test Lndr API data from local source instead of executing real API call'),
  );

  return system_settings_form($form);
}

/**
 * Form validation handler
 */
function lndr_admin_settings_validate($form, &$form_state) {
  // It be better if Lndr can validate the API token
  // @todo: implement the validate token method from Lndr 
}