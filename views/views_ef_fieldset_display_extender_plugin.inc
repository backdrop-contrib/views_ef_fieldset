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
      'type' => array('default' => 'container'),
      'options' => array(),
    );

    return $options;
  }

  /**
   * Provide a form to edit options for this plugin.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

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

    $form['views_ef_fieldset']['type'] = array(
      '#type' => 'select',
      '#title' => t('Type of fieldsets'),
      '#options' => array('container' => 'Simple container', 'vertical_tabs' => 'Vertical tabs'),
      '#default_value' => isset($options['type']) ?
        $options['type'] :
        $defaults['views_ef_fieldset']['type']['default'],
      '#states' => array(
        'visible' => array(
          ':input[name="views_ef_fieldset[enabled]"]' => array('checked' => TRUE),
        ),
      ),
    );

    $form['views_ef_fieldset']['options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Exposed form fieldsets'),
      '#states' => array(
        'visible' => array(
          ':input[name="views_ef_fieldset[enabled]"]' => array('checked' => TRUE),
        ),
      ),
    );

    $exposed_fields = array_values($this->view->display_handler->get_handlers('filter'));
    foreach ($exposed_fields as $exposed_field_index => $exposed_field) {
      if (!$exposed_field->options['exposed']) {
        unset($exposed_fields[$exposed_field_index]);
      }
    }

    $sort_fields = array_values($this->view->display_handler->get_handlers('sort'));
    foreach($sort_fields as $sort_fields_index => $sort_field) {
      if (!$sort_field->options['exposed']) {
        unset($sort_fields[$sort_fields_index]);
      }
    }

    if (count($sort_fields)) {
      $sort_by = new Stdclass();
      $sort_by->field = 'sort_by';
      $sort_by->handler_type = 'sort';
      $sort_by->options = array(
        'id' => 'sort_by',
        'expose' => array(
          'label' => 'Sort by'
        )
      );

      $exposed_fields[] = $sort_by;
    }

    if ($form['exposed_form_options']['expose_sort_order']['#default_value'] == 1) {
      $sort_order = new Stdclass();
      $sort_order->field = 'sort_order';
      $sort_order->handler_type = 'sort';
      $sort_order->options = array(
        'id' => 'sort_order',
        'expose' => array(
          'label' => 'Sort order'
        )
      );

      $exposed_fields[] = $sort_order;
    }

    $submit_button = new Stdclass();
    $submit_button->field = 'submit_button';
    $submit_button->handler_type = 'buttons';
    $submit_button->options = array(
      'id' => 'submit',
      'expose' => array(
        'label' => 'Submit button'
      )
    );
    $exposed_fields[] = $submit_button;

    if ($form['exposed_form_options']['reset_button']['#default_value'] == 1) {
      $reset_button = new Stdclass();
      $reset_button->field = 'reset_button';
      $reset_button->handler_type = 'buttons';
      $reset_button->options = array(
        'id' => 'reset',
        'expose' => array(
          'label' => 'Reset button'
        )
      );
      $exposed_fields[] = $reset_button;
    }

    foreach ($exposed_fields as $exposed_field_index => $exposed_field) {
      $fieldset = new StdClass();
      $fieldset->field = 'fieldset';
      $fieldset->handler_type = 'fieldset';
      $fieldset->options = array(
        'id' => 'fieldset' . $exposed_field_index,
        'expose' => array(
          'label' => 'Fieldset ' . $exposed_field_index
        )
      );
      $exposed_fields[] = $fieldset;
    }

    $data = array();
    foreach($exposed_fields as $field) {
      $field_options = $options['options']['sort'][$field->options['id']];
      $label = ($field->options['expose']['label']) ? $field->options['expose']['label'] : $field->options['id'];

      $data[] = array(
        'id' => $field->options['id'],
        'type' => $field->handler_type,
        'weight' => isset($field_options['weight']) ? (int) $field_options['weight'] : 0,
        'pid' => $field_options['pid'],
        'label' => $label,
        'title' => isset($field_options['title']) ? $field_options['title'] : $label,
        'description' => isset($field_options['description']) ? $field_options['description'] : '',
        'collapsible' => $field_options['collapsible'] ? $field_options['collapsible'] : FALSE,
        'collapsed' => $field_options['collapsed'] ? $field_options['collapsed'] : FALSE,
      );
    }

    $table_rows = array();
    // Get all root items.
    $root = _views_ef_fieldset_get_data($data, 'pid');

    foreach($root as $item) {
      $fitem = array(
        'weight' => array(
          '#item' => $item,
          '#type' => 'weight',
          '#title' => isset($options['options']['sort'][$item['id']]['title']) ? $options['options']['sort'][$item['id']]['title'] : $item['label'],
          '#delta' => count($data),
          '#title_display' => 'invisible',
          '#default_value' => $item['weight'],
          '#attributes' => array(
            'class' => array('item-weight')
          ),
        ),
        'type' => array(
          '#type' => 'hidden',
          '#default_value' => $item['type'],
        ),
        'id' => array(
          '#type' => 'hidden',
          '#default_value' => $item['id'],
          '#size' => 4,
          '#attributes' => array(
            'class' => array('item-id')
          ),
        ),
        'pid' => array(
          '#type' => 'hidden',
          '#default_value' => $item['pid'],
          '#size' => 4,
          '#attributes' => array(
            'class' => array('item-pid')
          )
        ),
        'depth' => array(
          '#type' => 'hidden',
          '#default_value' => 0,
        ),
      );
      if ($item['type'] = 'fieldset') {
        $fitem += array(
          'title' => array(
            '#type' => 'textfield',
            '#size' => 20,
            '#default_value' => isset($options['options']['sort'][$item['id']]['title']) ? $options['options']['sort'][$item['id']]['title'] : $item['title'],
          ),
          'description' => array(
            '#type' => 'textfield',
            '#size' => 20,
            '#default_value' => $item['description'],
          ),
          'collapsible' => array(
            '#type' => 'checkbox',
            '#default_value' => (bool) $item['collapsible'],
          ),
          'collapsed' => array(
            '#type' => 'checkbox',
            '#default_value' => (bool) $item['collapsed'],
          )
        );
      }

      $table_rows[$item['id']] = $fitem;

      // Get items having a parent.
      $fields = _views_ef_fieldset_get_data($data, 'pid', $item['id']);
      foreach($fields as $item) {
        $fitem = array(
          'weight' => array(
            '#item' => $item,
            '#type' => 'weight',
            '#title' => $item['label'],
            '#delta' => count($data),
            '#title_display' => 'invisible',
            '#default_value' => $item['weight'],
            '#attributes' => array(
              'class' => array('item-weight', 'subitem-weight')
            ),
          ),
          'type' => array(
            '#type' => 'hidden',
            '#default_value' => $item['type'],
          ),
          'id' => array(
            '#type' => 'hidden',
            '#default_value' => $item['id'],
            '#size' => 4,
            '#attributes' => array(
              'class' => array('item-id')
            ),
          ),
          'pid' => array(
            '#type' => 'hidden',
            '#default_value' => $item['pid'],
            '#size' => 4,
            '#attributes' => array(
              'class' => array('item-pid')
            )
          ),
          'depth' => array(
            '#type' => 'hidden',
            '#default_value' => (empty($item['pid']) ? '' : 1),
          ),
        );

        $table_rows[$item['id']] = $fitem;
      }
    }

    // Get items not in a fieldset
    $data2 = _views_ef_fieldset_get_data($data, 'type', 'fieldset', '!=');
    $fields = _views_ef_fieldset_get_data($data2, 'pid');
    foreach($fields as $item) {
      $fitem = array(
        'weight' => array(
          '#item' => $item,
          '#type' => 'weight',
          '#title' => $item['label'],
          '#delta' => count($data),
          '#title_display' => 'invisible',
          '#default_value' => $item['weight'],
          '#attributes' => array(
            'class' => array('item-weight')
          ),
        ),
        'type' => array(
          '#type' => 'hidden',
          '#default_value' => $item['type'],
        ),
        'id' => array(
          '#type' => 'hidden',
          '#default_value' => $item['id'],
          '#size' => 4,
          '#attributes' => array(
            'class' => array('item-id')
          ),
        ),
        'pid' => array(
          '#type' => 'hidden',
          '#default_value' => $item['pid'],
          '#size' => 4,
          '#attributes' => array(
            'class' => array('item-pid')
          )
        ),
        'depth' => array(
          '#type' => 'hidden',
          '#default_value' => (empty($item['pid']) ? '' : 1),
        ),
      );
      $table_rows[$item['id']] = $fitem;
    }

    $form['views_ef_fieldset']['options']['sort'] = $table_rows;
    $form['views_ef_fieldset']['options']['sort']['#theme'][] = 'views_ef_fieldset_reorder_form';
  }

  function options_submit(&$form, &$form_state) {
    $this->display->set_option('views_ef_fieldset', $form_state['values']['views_ef_fieldset']);
    parent::options_submit($form, $form_state);
  }
}
