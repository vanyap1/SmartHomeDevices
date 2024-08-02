using HidLibrary;
using System;
using System.Threading;


class Program
{
    static void Main(string[] args)
    {
        HidDevice device = HidDevices.Enumerate(0x03EB, 0x2402).FirstOrDefault();

        if (device == null)
        {
            Console.WriteLine("Device can't find");
            return;
        }

        if (!device.IsConnected)
        {
            return;
        }

        byte[] dataToSend = new byte[64];
        for (byte i = 0; i < 63; i++)
        {
            dataToSend[i+1] = i;
        }
        dataToSend[2] = 1;
        while (true)
        {
            try
            {
                device.OpenDevice();
                if (device.IsConnected)
                {
                    device.Write(dataToSend);
                    dataToSend[2]++;
                    if (dataToSend[2] >= 3){
                        dataToSend[2] = 1;
                    }

                    byte[] reportBuffer = new byte[64];
                    HidReport report = device.ReadReport();
                    if (report.ReadStatus == HidDeviceData.ReadStatus.Success)
                    {
                        reportBuffer = report.Data;
                        foreach (byte b in reportBuffer)
                        {
                            Console.Write(b.ToString("X2"));
                        }
                        Console.WriteLine();
                    }
                    else
                    {
                        Console.WriteLine("Data read error");
                    }
                    device.CloseDevice();
                }
            }catch (Exception ex) { 
                Console.WriteLine($"{ex.Message}");
            }
            Thread.Sleep(250);
            }
    }
}