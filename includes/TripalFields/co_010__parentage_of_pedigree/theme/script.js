/**
 * @file
 * Manage parentage pedigree behavior
 */
(function ($) {
Drupal.behaviors.ParentagePedigree = {
  attach: function(context, settings) {
    var stockName =  Drupal.settings.trpfancyFields.name;
    var url       =  Drupal.settings.trpfancyFields.url;
    var pedigreeChart =  Drupal.settings.trpfancyFields.chart;

    var maxDepth, width = 0;

    $.getJSON(url.path_JSON, function(data) {

      // Calculate the height of the diagram based on the depth of the tree.
      // Assumption: distance between nodes is ~75px
      getDepth = function (obj) {
        var depth = 0;
        if (obj.children) {
          obj.children.forEach(function (d) {
            var tmpDepth = getDepth(d)

            if (tmpDepth > depth) {
              depth = tmpDepth
            }
          })
        }

        return 1 + depth
      }

      maxDepth = getDepth(data[0]);

      // Calculate the width of the diagram based on the overview pane.
      // We need to do this b/c the hidden div that will contain the tree
      // has width=0 at this point ;-P.

      // Find the table (summary/overivew) in one of the tripal-panes not hidden.
      // Set the <table> to occupy the entire pane and use the width as the
      // estimated width of the SVG element.
      $('.ds-right div:visible').find('table').css('width', '100%');
      width = Math.floor($('.ds-right div:visible').find('table').innerWidth());

      if (width == 0) {
        // All else fail, set to 750;
        width = 750;
        console.error('Unable to detect width thus defaulting to 750px.');
      }

      // The following code uses the Tripal D3 module to draw a pedigree tree
      // and attach it to an #tree element. Furthermore, it specifies a path
      // to get the data for the tree from.
      tripalD3.drawFigure(data, {
        'chartType'      : 'pedigree',
        'elementId'      : pedigreeChart.element,
        'height'         : 75 * maxDepth,
        'width'          : width,
        'title'          : '<em>' + stockName + '</em> Parental Pedigree',
        'legend'         : 'The above tree depicts the parentage of <em>' + stockName + '</em>. The type of relationship is indicated both using line styles defined in the legend and also in sentence form when you hover your mouse over the relationship lines. Additional information about each germplasm can be obtained by clicking on the Germplasm name. Furthermore, parts of the pedigree diagram can be collapsed or expanded by double-clicking on a Germplasm node.',

        // Do not render the tree legend on the upper left hand corner
        // since we want it on top.
        'drawKey'        : pedigreeChart.draw_key_legend,
        'keyPosition'    : pedigreeChart.key_legend_position,

        // On load, collapse level from the end leaf down.
        // Levels:
        'collapsedDepth' : pedigreeChart.collapse_level,
        //
        'pass'           : Drupal.settings.trpfancyFields.pass,
      });


      $('#tripald3-pedigree-wait').remove();
    });
}}}(jQuery));
