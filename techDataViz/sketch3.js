var sketch3 = (function () { //use IIFE to avoid variable name collision
    var companyLabelWidth = 120; // space reserved for bar labels
    var gridLabelHeight = 18; // space reserved for gridline labels
    var gridChartOffset = 10; // space between start of grid and first bar
    var maxBarWidth = 700; // width of the bar with the max value
    var exec = false;
    var width = 900;
    var height = 716;

    var svg = d3.select("#plot3").append("svg")
        .attr("width", width)
        .attr("height", height),
        g = svg.append("g")
        .attr('transform', 'translate(' + companyLabelWidth + ',' + (gridLabelHeight + gridChartOffset) + ')');

    var x = d3.scaleLinear()
        .rangeRound([0, maxBarWidth]);

    var y = d3.scaleBand()
        .rangeRound([0, height])
        .padding(0.35)
        .align(0.1);

    var colors = d3.scaleOrdinal()
        .range(["#007F75", "#fa654b"]);

    var stack = d3.stack()
        .offset(d3.stackOffsetExpand);

    var tooltip = d3.select("#plot3").append("div")
        .attr("class", "toolTip")
        .style("display", "none");

    var thisData;

    // add the X gridlines
    g.append("g")
        .attr("class", "grid")
        .attr("transform", "translate(0," + (height + 3) + ")")
        .call(d3.axisBottom(x)
            .ticks(10)
            .tickSize(-height)
            .tickFormat("")
        ).selectAll("line").style("stroke", "#ccc");

    var axisX = g.append("g")
        .attr("class", "axis axis--x")
        .call(d3.axisTop(x).ticks(10, "%"));

    axisX.selectAll("text")
        .style("fill", "#282828")
        .attr("font-size", "15px")
        .attr("font-family", "Work Sans");

    var axisY = g.append("g")
        .attr("class", "axis axis--y")
        .call(d3.axisLeft(y));

    axisY.selectAll("text")
        .style("fill", "#282828")
        .attr("font-size", "15px")
        .attr("font-family", "Work Sans");

    axisY.selectAll("line")
        .style("stroke", "transparent");

    // get the data
    d3.csv("data/diversity.csv", type, function (error, data) {
        console.log(data);
        draw(); //initial draw

        d3.selectAll("#execSwitch").on("change", function () {
            if (exec == false) {
                exec = true;
                g.selectAll(".bars").remove();
                draw(exec);
            } else {
                exec = false;
                g.selectAll(".bars").remove();
                draw(exec);
            }
            console.log(exec);
        })

        d3.selectAll("#sort").on("change", function () {
            if (this.value == "name") {
                data.sort(function (a, b) {
                    return d3.descending(b.Company, a.Company);
                });
            } else if (this.value == "female") {
                if (exec) {
                    data.sort(function (a, b) {
                        return d3.ascending(b.FemaleLeadership, a.FemaleLeadership);
                    });
                } else {
                    data.sort(function (a, b) {
                        return d3.ascending(b.Female, a.Female);
                    });
                }

            }
            g.selectAll(".bars").remove(); //remove all bars before redraw
            draw(exec);
        });

        if (error) throw error;

        function draw(executive) {
            y.domain(data.map(function (d) {
                return d.Company;
            }));

            axisY.transition()
                .call(d3.axisLeft(y));

            if (executive) {
                var bars = g.selectAll(".bars")
                    .data(stack.keys(data.columns.slice(3, 6))(data))
                    .enter().append("g")
                    .attr("class", "bars")
                    .attr("fill", function (d) {
                        return colors(d.key);
                    });
            } else {
                var bars = g.selectAll(".bars")
                    .data(stack.keys(data.columns.slice(1, 3))(data))
                    .enter().append("g")
                    .attr("class", "bars")
                    .attr("fill", function (d) {
                        return colors(d.key);
                    });
            }


            var rects = bars.selectAll("rect")
                .data(function (d) {
                    return d;
                })
                .enter().append("rect")
                .attr("x", function (d) {
                    return x(d[0]);
                })
                .attr("width", function (d) {
                    return x(d[1]) - x(d[0]);
                })
                .attr("height", y.bandwidth())
                .attr("y", function (d) {
                    return y(d.data.Company);
                })

            rects.on("mousemove", function (d) {
                    tooltip
                        .style("left", d3.event.pageX + 10 + "px")
                        .style("top", d3.event.pageY - 70 + "px")
                        .style("display", "inline-block")

                    if (exec) {
                        tooltip.html(d.data.Company + " - Leadership<br>" +
                            "Female: " + d.data.FemaleLeadership + "%<br>" +
                            "Male: " + d.data.MaleLeadership + "%");
                    } else {
                        tooltip.html(d.data.Company + " - All Employees<br>" +
                            "Female: " + d.data.Female + "%<br>" +
                            "Male: " + d.data.Male + "%<br>");
                    }
                })
                .on("mouseout", function (d) {
                    tooltip.style("display", "none");
                });

            colors.domain(data.columns.slice(1));
        }
    });

    function type(d, i, columns) {
        for (i = 1, t = 0; i < columns.length; ++i) {
            t += d[columns[i]] = +d[columns[i]];
            d.total = 100;
        }
        return d;
    }

})();

sketch3;