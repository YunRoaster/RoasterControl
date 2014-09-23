$(document).ready(function(){
    // Configuration
    var palette = new Rickshaw.Color.Palette();
    var initial_data = [
        {name: 'power'},
        {name: 'bean'},
        {name: 'env'}
    ];

    time_plot = new Rickshaw.Graph({
        element: document.querySelector("#chart"),
        width: 540,
        height: 240,
        min: 0,
        max: 550,
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

    var y_axis = new Rickshaw.Graph.Axis.Y({
        graph: time_plot,
        orientation: 'left',
        tickFormat: Rickshaw.Fixtures.Number.formatKMBT,
        element: document.getElementById('y_axis'),
    });

    var legend = new Rickshaw.Graph.Legend({
        element: document.querySelector('#legend'),
        graph: time_plot
    });

    // The plot is ready to receive data
    time_plot.render();
    time_plot_ready = true;
});