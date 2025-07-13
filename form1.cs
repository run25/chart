using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace TouchFriendlyChart
{
    public partial class Form1 : Form
    {
        class GraphInfo
        {
            public string Name;
            public List<double> Values = new List<double>();
            public double YMin = double.MaxValue;
            public double YMax = double.MinValue;
            public Color LineColor;
        }

        List<DateTime> timestamps = new List<DateTime>();
        List<GraphInfo> graphs = new List<GraphInfo>();
        Random rand = new Random();

        public Form1()
        {
            InitializeComponent();
            InitChart();
        }

        private void InitChart()
        {
            chart1.Series.Clear();
            chart1.ChartAreas.Clear();

            var area = new ChartArea("MainArea");
            area.AxisX.LabelStyle.Format = "HH:mm";
            area.AxisX.MajorGrid.LineColor = Color.LightGray;

            // タッチ操作向け：ズーム・パン設定
            area.AxisX.ScaleView.Zoomable = true;
            area.AxisX.ScrollBar.Enabled = false; // ボタン操作に統一
            area.CursorX.IsUserEnabled = true;
            area.CursorX.IsUserSelectionEnabled = true;

            area.AxisY.ScaleView.Zoomable = true;
            area.AxisY.ScrollBar.Enabled = false;
            area.CursorY.IsUserEnabled = true;
            area.CursorY.IsUserSelectionEnabled = true;

            chart1.ChartAreas.Add(area);
            chart1.Legends[0].Enabled = true;

            // チェックリストイベント
            checkedListBox1.ItemCheck += (s, e) =>
            {
                string name = checkedListBox1.Items[e.Index].ToString();
                bool show = e.NewValue == CheckState.Checked;
                if (chart1.Series.IndexOf(name) >= 0)
                    chart1.Series[name].Enabled = show;
            };
        }

        private void btnLoadCsv_Click(object sender, EventArgs e)
        {
            var ofd = new OpenFileDialog();
            ofd.Filter = "CSVファイル|*.csv";
            if (ofd.ShowDialog() != DialogResult.OK) return;

            LoadCsv(ofd.FileName);
            DrawChart();
        }

        private void LoadCsv(string path)
        {
            timestamps.Clear();
            graphs.Clear();

            using (var reader = new StreamReader(path))
            {
                var headers = reader.ReadLine().Split(',');

                for (int i = 1; i < headers.Length; i++)
                {
                    graphs.Add(new GraphInfo
                    {
                        Name = headers[i],
                        LineColor = Color.FromArgb(rand.Next(50, 230), rand.Next(50, 230), rand.Next(50, 230))
                    });
                }

                while (!reader.EndOfStream)
                {
                    var line = reader.ReadLine();
                    var parts = line.Split(',');

                    if (!DateTime.TryParse(parts[0], out DateTime t)) continue;
                    timestamps.Add(t);

                    for (int i = 0; i < graphs.Count; i++)
                    {
                        if (i + 1 >= parts.Length) continue;
                        if (!double.TryParse(parts[i + 1], out double val)) val = 0;

                        graphs[i].Values.Add(val);
                        graphs[i].YMin = Math.Min(graphs[i].YMin, val);
                        graphs[i].YMax = Math.Max(graphs[i].YMax, val);
                    }
                }
            }

            checkedListBox1.Items.Clear();
            foreach (var g in graphs)
                checkedListBox1.Items.Add(g.Name, true);
        }

        private void DrawChart()
        {
            chart1.Series.Clear();
            var area = chart1.ChartAreas["MainArea"];

            double globalMin = double.MaxValue;
            double globalMax = double.MinValue;

            foreach (var g in graphs)
            {
                var s = new Series(g.Name)
                {
                    ChartType = SeriesChartType.Line,
                    ChartArea = "MainArea",
                    XValueType = ChartValueType.DateTime,
                    Color = g.LineColor,
                    BorderWidth = 2
                };

                for (int i = 0; i < timestamps.Count && i < g.Values.Count; i++)
                    s.Points.AddXY(timestamps[i], g.Values[i]);

                chart1.Series.Add(s);

                globalMin = Math.Min(globalMin, g.YMin);
                globalMax = Math.Max(globalMax, g.YMax);
            }

            if (Math.Abs(globalMax - globalMin) < 1e-8)
            {
                globalMin -= 1;
                globalMax += 1;
            }

            area.AxisY.Minimum = globalMin;
            area.AxisY.Maximum = globalMax;

            if (timestamps.Count > 10)
            {
                double start = timestamps.First().ToOADate();
                double end = timestamps.Last().ToOADate();
                double width = end - start;
                double viewSize = width * 0.2;
                area.AxisX.Minimum = start;
                area.AxisX.Maximum = end;
                area.AxisX.ScaleView.Zoom(start, start + viewSize);
            }
        }

        private void ScrollChartX(double shiftRatio)
        {
            var area = chart1.ChartAreas["MainArea"];
            var view = area.AxisX.ScaleView;

            if (!view.IsZoomed) return;

            double size = view.ViewMaximum - view.ViewMinimum;
            double shift = size * shiftRatio;

            double newStart = view.ViewMinimum + shift;
            double newEnd = view.ViewMaximum + shift;

            if (newEnd > area.AxisX.Maximum)
            {
                newEnd = area.AxisX.Maximum;
                newStart = newEnd - size;
            }
            if (newStart < area.AxisX.Minimum)
            {
                newStart = area.AxisX.Minimum;
                newEnd = newStart + size;
            }

            view.Zoom(newStart, newEnd);
        }

        private void btnLeft_Click(object sender, EventArgs e)
        {
            ScrollChartX(-0.2);
        }

        private void btnRight_Click(object sender, EventArgs e)
        {
            ScrollChartX(0.2);
        }
    }
}
