<?php
/**
 * @file
 * Module file containing theme implementations.
 */

/**
 * Theme callback to render the option form as a draggable table.
 */
function theme_views_ef_fieldset_reorder_form($vars) {
  $form = $vars['form'];
  $table_rows = array();

  foreach (element_children($form) as $key) {
    $classes = array('draggable');
    $row = array();
    $element = $form[$key];
    $field = $element['weight']['#item'];

    $indent = theme('indentation', array('size' => $element['depth']['#default_value']));
    unset($element['depth']);

    if ($element['type']['#default_value'] == 'fieldset') {
      $title = '<strong>' . $element['weight']['#title'] . '</strong>';
    } else {
      $title = $element['weight']['#title'];
    }

    if ($field['type'] == 'fieldset') {
      $classes[] = 'tabledrag-root';
    } else {
      $classes[] = 'tabledrag-leaf';
    }

    $row[] = $indent . $title . drupal_render($element['id']) . drupal_render($element['pid']);
    $row[] = drupal_render($element['title']);
    $row[] = drupal_render($element['description']);
    $row[] = drupal_render($element['collapsible']);
    $row[] = drupal_render($element['collapsed']);
    $row[] = drupal_render($element['weight']);

    $table_rows[] = array('data' => $row, 'class' => $classes, 'id' => 'display-row-' . $key);
  }

  drupal_add_tabledrag('reorder-fieldset-filters', 'order', 'sibling', 'item-weight');
  drupal_add_tabledrag('reorder-fieldset-filters', 'match', 'parent', 'item-pid', 'item-pid', 'item-id', FALSE);

  $header = array(t('Item'), t('Title'), t('Description'), t('Collapsible'), t('Collapsed'), '');

  $output = '';
  $output .= theme('table',
    array(
      'header' => $header,
      'rows' => $table_rows,
      'attributes' => array('id' => 'reorder-fieldset-filters'),
    ));

  return $output;
}
