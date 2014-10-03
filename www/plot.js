$(document).ready(function(){
    // Configuration
    var palette = new Rickshaw.Color.Palette();
    // Each is ~x50?
    var tempScale = d3.scale.linear().domain([0, 10]).nice();
    var powerScale = d3.scale.linear().domain([0, 2]).nice();
    var initial_data = [
        {name: 'power', scale: powerScale},
        {name: 'bean', scale: tempScale},
        {name: 'env', scale: tempScale}
    ];

    time_plot = new Rickshaw.Graph({
        element: document.querySelector("#chart"),
        width: 540,
        height: 240,
        renderer: 'line',
        series: new Rickshaw.Series.FixedDuration(initial_data, palette, {
            timeInterval: 2000,
            maxDataPoints: 100,
            timeBase: Math.floor(+new Date().getTime() / 100)/10
        })
    });

    var x_axis = new Rickshaw.Graph.Axis.Time({
        graph: time_plot
    });

    var temp_axis = new Rickshaw.Graph.Axis.Y.Scaled({
        graph: time_plot,
        grid: false,
        pixelsPerTick: 30,
        orientation: 'left',
        tickFormat: Rickshaw.Fixtures.Number.formatKMBT,
        scale: tempScale,
        element: document.getElementById('temp_axis'),
    });

    var power_axis = new Rickshaw.Graph.Axis.Y.Scaled({
      graph: time_plot,
      grid: false,
      pixelsPerTick: 30,
      orientation: 'right',
      scale: powerScale,
      tickFormat: Rickshaw.Fixtures.Number.formatKMBT,
      element: document.getElementById('power_axis')
    });

    var legend = new Rickshaw.Graph.Legend({
        element: document.querySelector('#legend'),
        graph: time_plot
    });

    new Rickshaw.Graph.HoverDetail({
      graph: time_plot
    });

    // The plot is ready to receive data
    time_plot.render();
    time_plot_ready = true;
});