<?php
/**
 * @file
 * Provides a display extender plugin for View 3.x.
 */

class views_ef_fieldset_display_extender_plugin extends views_plugin_display_extender {

  function option_definition() {
    $options = parent::option_definition();

    $options['views_ef_fieldset'] = array(
      'enabled' => array('default' => FALSE, 'bool' => TRUE),
      'options' => array(
        'title' => array('default' => 'Filters', 'translatable' => TRUE),
        'description' => array('default' => '', 'translatable' => TRUE),
        'collapsible' => array('default' => TRUE, 'bool' => TRUE),
        'collapsed' => array('default' => FALSE, 'bool' => TRUE),
      )
    );

    return $options;
  }

  /**
   * Provide a form to edit options for this plugin.
   */
  function options_form(&$form, &$form_state) {
    if ($form['#section'] != 'default-exposed_form_options') {
      return;
    }

    $options = $this->display->get_option('views_ef_fieldset');
    $defaults = $this->option_definition();

    $form['views_ef_fieldset'] = array(
      '#tree' => TRUE,
    );

    $form['views_ef_fieldset']['enabled'] = array(
      '#type' => 'checkbox',
      '#default_value' => isset($options['enabled']) ?
        $options['enabled'] :
        $defaults['views_ef_fieldset']['enabled']['default'],
      '#title' => t('Enable fieldset around exposed forms ?'),
    );

    $form['views_ef_fieldset']['options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Exposed form fieldset options'),
      '#states' => array(
        'visible' => array(
          ':input[name="views_ef_fieldset[enabled]"]' => array('checked' => TRUE),
        ),
      ),
    );

    $form['views_ef_fieldset']['options']['title'] = array(
      '#type' => 'textfield',
      '#default_value' => isset($options['options']['title']) ?
        $options['options']['title'] :
        $defaults['views_ef_fieldset']['options']['title']['default'],
      '#title' => t('Title of the fieldset'),
    );

    $form['views_ef_fieldset']['options']['description'] = array(
      '#type' => 'textfield',
      '#default_value' => isset($options['options']['description']) ?
        $options['options']['description'] :
        $defaults['views_ef_fieldset']['options']['description']['default'],
      '#title' => t('Description in the fieldset'),
    );

    $form['views_ef_fieldset']['options']['collapsible'] = array(
      '#type' => 'checkbox',
      '#default_value' => isset($options['options']['collapsible']) ?
        $options['options']['collapsible'] :
        $defaults['views_ef_fieldset']['options']['collapsible']['default'],
      '#title' => t('Should it be collapsible ?'),
    );

    $form['views_ef_fieldset']['options']['collapsed'] = array(
      '#type' => 'checkbox',
      '#default_value' => isset($options['options']['collapsed']) ?
        $options['options']['collapsed'] :
        $defaults['views_ef_fieldset']['options']['collapsed']['default'],
      '#title' => t('Should it be collapsed ?'),
    );
  }

  function options_submit(&$form, &$form_state) {
    $this->display->set_option('views_ef_fieldset', $form_state['values']['views_ef_fieldset']);
    parent::options_submit($form, $form_state);
  }


  function query() {
    $options = $this->display->get_option('views_ef_fieldset');

    if ($options['enabled'] && !empty($this->view->exposed_widgets)) {
      $classes = array();

      if ($options['options']['collapsible'] == TRUE) {
        $classes[] = 'collapsible';
        drupal_add_library('system', 'drupal.collapse');
      }
      if ($options['options']['collapsed'] == TRUE) {
        $classes[] = 'collapsed';
      }

      $this->view->exposed_widgets = array(
        '#type' => 'fieldset',
        'exposed_widgets' => array(
          '#markup' => $this->view->exposed_widgets
        )
      );

      if (!empty($options['options']['title'])) {
        $this->view->exposed_widgets += array(
          '#title' => check_plain($options['options']['title']),
        );
      }

      if (!empty($options['options']['description'])) {
        $this->view->exposed_widgets += array(
          '#description' => check_plain($options['options']['description']),
        );
      }

      if (!empty($classes)) {
        $this->view->exposed_widgets += array(
          '#attributes' => array('class' => $classes)
        );
      }

      $this->view->exposed_widgets = drupal_render($this->view->exposed_widgets);
    }
    parent::query();
  }

}
