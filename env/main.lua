app.Init()
app.resource.LoadNodeSet("BaseNodeSet")
app.viewer.InitThreadPool(4)
app.viewer.LoadPipeline("defferred")
app.viewer.InitPipeline()
app.viewer.LoadScene("scene1")

app.Loop()

app.viewer.DestroyPipeline()
PrintLn("End Game")
while true do
end