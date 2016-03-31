' The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

''' <summary>
''' An empty page that can be used on its own or navigated to within a Frame.
''' </summary>
''' 
Imports Windows.Web.Http
Imports Windows.Data.Json

Public NotInheritable Class MainPage
    Inherits Page
    Const constDevice As String = "http://192.168.1.37/"
    Public bolUpdatingUI As Boolean = False  'used to prevent update loops on sliders when updating their positions in code

    Public Sub UpdateUI(strStatus As String)

        Dim TankStatus As JsonObject
        TankStatus = JsonValue.Parse(strStatus).GetObject
        bolUpdatingUI = True
        'led
        If TankStatus("led").GetString = -1 Then
            Me.btnTurnLightsAuto.Background = New SolidColorBrush(Windows.UI.Colors.Lime)
            Me.btnTurnLightsManual.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
            Me.btnTurnLightsOff.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
        End If
        If TankStatus("led").GetString = 0 Then
            Me.btnTurnLightsAuto.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
            Me.btnTurnLightsManual.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
            Me.btnTurnLightsOff.Background = New SolidColorBrush(Windows.UI.Colors.Lime)
        End If
        If TankStatus("led").GetString = 1 Then
            Me.btnTurnLightsAuto.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
            Me.btnTurnLightsManual.Background = New SolidColorBrush(Windows.UI.Colors.Lime)
            Me.btnTurnLightsOff.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
        End If
        slRedLED.Value = TankStatus("rled").GetString
        slGreenLED.Value = TankStatus("gled").GetString
        slBlueLED.Value = TankStatus("bled").GetString
        tbTankTempData.Text = TankStatus("tanktemp").GetString
        tbDateTimeData.Text = TankStatus("tankdate").GetString & " " & TankStatus("tanktime").GetString
        'lastreboot
        tbLastRebootData.Text = TankStatus("lastreboot").GetString

        'fanmode
        If TankStatus("fanmode").GetString = -1 Then
            Me.btnTurnFanOn.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
            Me.btnTurnFanOff.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
            Me.btnTurnFanAuto.Background = New SolidColorBrush(Windows.UI.Colors.Lime)
        End If
        If TankStatus("fanmode").GetString = 0 Then
            Me.btnTurnFanOn.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
            Me.btnTurnFanOff.Background = New SolidColorBrush(Windows.UI.Colors.Lime)
            Me.btnTurnFanAuto.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
        End If
        If TankStatus("fanmode").GetString = 1 Then
            Me.btnTurnFanOn.Background = New SolidColorBrush(Windows.UI.Colors.Lime)
            Me.btnTurnFanOff.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
            Me.btnTurnFanAuto.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
        End If

        'fan

        'pump
        If TankStatus("pump").GetString = 1 Then
            Me.btnTurnPumpOn.Background = New SolidColorBrush(Windows.UI.Colors.Lime)
            Me.btnTurnPumpOff.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
        Else
            Me.btnTurnPumpOff.Background = New SolidColorBrush(Windows.UI.Colors.Lime)
            Me.btnTurnPumpOn.Background = New SolidColorBrush(Windows.UI.Colors.LightGray)
        End If
        bolUpdatingUI = False

    End Sub


    Private Async Sub btnGetStatus_Click(sender As Object, e As RoutedEventArgs) Handles btnGetStatus.Click

        Dim strResponse As String
        strResponse = Await GetTankStatus()
        UpdateUI(strResponse)

    End Sub

    Private Async Sub btnTurnLightsAuto_click(sender As Object, e As RoutedEventArgs) Handles btnTurnLightsAuto.Click

        Dim strResponse As String
        strResponse = Await SendCommand("led=-1")
        UpdateUI(strResponse)

    End Sub

    Private Async Sub btnTurnLightsOff_click(sender As Object, e As RoutedEventArgs) Handles btnTurnLightsOff.Click

        Dim strResponse As String
        strResponse = Await SendCommand("led=0")
        UpdateUI(strResponse)

    End Sub

    Private Async Sub btnTurnLightsManual_click(sender As Object, e As RoutedEventArgs) Handles btnTurnLightsManual.Click

        Dim strResponse As String
        strResponse = Await SendCommand("led=1")
        UpdateUI(strResponse)

    End Sub

    Private Async Sub btnTurnFanOn_Click(sender As Object, e As RoutedEventArgs) Handles btnTurnFanOn.Click

        Dim strResponse As String
        strResponse = Await SendCommand("fan=1")
        UpdateUI(strResponse)

    End Sub
    Private Async Sub btnTurnFanOff_Click(sender As Object, e As RoutedEventArgs) Handles btnTurnFanOff.Click

        Dim strResponse As String
        strResponse = Await SendCommand("fan=0")
        UpdateUI(strResponse)

    End Sub
    Private Async Sub btnTurnFanAuto_Click(sender As Object, e As RoutedEventArgs) Handles btnTurnFanAuto.Click

        Dim strResponse As String
        strResponse = Await SendCommand("fan=-1")
        UpdateUI(strResponse)

    End Sub

    Private Async Sub btnTurnPumpOn_Click(sender As Object, e As RoutedEventArgs) Handles btnTurnPumpOn.Click

        Dim strResponse As String
        strResponse = Await SendCommand("pump=1")
        UpdateUI(strResponse)

    End Sub
    Private Async Sub btnTurnPumpOff_Click(sender As Object, e As RoutedEventArgs) Handles btnTurnPumpOff.Click

        Dim strResponse As String
        strResponse = Await SendCommand("pump=0")
        UpdateUI(strResponse)

    End Sub

    Private Async Sub slRedLed_Change(sender As Object, e As RoutedEventArgs) Handles slRedLED.ValueChanged

        If Not bolUpdatingUI Then
            Dim strResponse As String
            strResponse = Await SendCommand("rled=" & slRedLED.Value)
        End If

    End Sub

    Private Async Sub slGreenLed_Change(sender As Object, e As RoutedEventArgs) Handles slGreenLED.ValueChanged

        If Not bolUpdatingUI Then
            Dim strResponse As String
            strResponse = Await SendCommand("gled=" & slGreenLED.Value)
        End If

    End Sub
    Private Async Sub slBlueLED_Change(sender As Object, e As RoutedEventArgs) Handles slBlueLED.ValueChanged

        If Not bolUpdatingUI Then
            Dim strResponse As String
            strResponse = Await SendCommand("bled=" & slBlueLED.Value)
        End If

    End Sub


    Private Async Function SendCommand(strCommand As String) As Task(Of String)

        Dim myurl As String = constDevice + strCommand
        Dim myUri As Uri
        myUri = New Uri(myurl)
        Dim req As New HttpClient
        Dim res As HttpResponseMessage = Await req.GetAsync(myUri)
        Dim resText As String
        resText = Await res.Content.ReadAsStringAsync
        Return resText

    End Function

    Private Async Function GetTankStatus() As Task(Of String)

        Dim myurl As String = constDevice
        Dim myUri As Uri
        myUri = New Uri(myurl)
        Dim req As New HttpClient
        Dim res As HttpResponseMessage = Await req.GetAsync(myUri)
        Dim resText As String
        resText = Await res.Content.ReadAsStringAsync
        Return resText

    End Function

End Class
