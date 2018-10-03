<?php

/**
 * @file
 * Code for the Lndr administration pages.
 */

/**
 * Form builder callback.
 */
function lndr_admin_settings($form, &$form_state) {
  global $base_url;
  $form['lndr_base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Base URL'),
    '#default_value' => variable_get('lndr_base_url', $base_url),
    '#description' => t("Allows to override a base url setting."),
    '#required' => TRUE,
  );

  $form['lndr_token'] = array(
    '#type' => 'textfield',
    '#title' => t('API token'),
    '#default_value' => variable_get('lndr_token', ''),
    '#description' => t("Your Lndr account API token, you can find this information under your user profile in Lndr"),
    '#required' => TRUE,
  );

  if (module_exists('lndr_example')) {
    $form['lndr_debug_mode'] = array(
      '#type' => 'checkbox',
      '#title' => t('Developer mode'),
      '#default_value' => variable_get('lndr_debug_mode', 0),
      '#description' => t('Turn this on to test Lndr API data from local source instead of executing real API call'),
    );
  }

  $form['lndr_delivery_mode'] = array(
    '#type' => 'radios',
    '#title' => t('Content import mode'),
    '#options' => array(
      'content_only' => t('Content only (fastest)'),
      'all_assets' => t('All content and assets'),
    ),
    '#options_descriptions' => array(
      'content_only' => 'We will only import the index.html file while loading images and other assets from the Lndr server',
      'all_assets' => 'All of the Lndr page content and assets from the Lndr server will be imported to the Drupal website in your public file directory',
    ),
    '#after_build' => array('_lndr_option_descriptions'),
    '#default_value' => variable_get('lndr_delivery_mode', 'all_assets'),
  );

  return system_settings_form($form);
}

/**
 * Provide a description to each option in the element.
 */
function _lndr_option_descriptions($element, &$form_state) {
  foreach (element_children($element) as $key) {
    if (isset($element['#options_descriptions'][$key])) {
      $element[$key]['#description'] = t('!description', array(
        '!description' => $element['#options_descriptions'][$key],
      ));
    }
  }
  return $element;
}

/**
 * Form validation handler.
 */
function lndr_admin_settings_validate($form, &$form_state) {
  // Validate the user.
  $options = array(
    'method' => 'POST',
    'data' => 'token=' . $form_state['values']['lndr_token'],
  );
  $request = drupal_http_request(LNDR_API_VALIDATE_TOKEN, $options);
  if ($request->code !== '200') {
    form_set_error(
      'lndr_token',
      t('You have entered an invalid API token, please copy and paste the API token from your profile in Lndr')
    );
  }
  else {
    // @todo: in the future we can do additional check such as see if that user is inactive, etc.
    // @TODO We need use somewhere this variable.
    variable_set('lndr_api_info', json_decode($request->data, TRUE));
  }
}
