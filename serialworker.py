import serial
import time
import asyncio
import os
import threading
class ComPort():
    

    
    SERIAL_BAUDRATE = 115200
    PATH_TO_COM     = "/dev/"

    async def __task(self, msg):
        self.sp.write((msg + "\n").encode(encoding = 'ascii'))
        while True:

            await asyncio.sleep(1)
            dd = self.sp.inWaiting()
            
            # print(dd)
            if (dd > 2):
                print("check")
                data = self.sp.readline().decode('utf-8')
                print(data)
                return data

        
        pass    
    
    async def check_arduino(self):
        resp = await asyncio.gather(self.request("statinit, \n"))
        print(resp)
        return resp
    
    
    
    def close(self):
        self.sp.close()
    
    def tryInit(self):
        while True:
            time.sleep(4)
            files = os.listdir("/dev/")

            contr = list(filter(lambda x : x.find("ttyUSB") != -1, files))

            print(contr)
            if len(contr) != 0:
                            
                # dev = self.PATH_TO_COM + contr[0]
                self.ports = list(map(lambda x : self.PATH_TO_COM + contr[0] + x, contr))
                dev     = self.PATH_TO_COM + contr[0]
                self.sp = serial.Serial(dev, self.SERIAL_BAUDRATE, timeout=1)
                self.sp.flushInput()
                self.arduino_port_init = True
                break
                   
    
    def get_stat_arduino(self):
        if self.arduino_port_init:
            return {
                "status_init" : "succes",
                "dev"         : "\n".join(self.ports),
            }
        else:
            return {
                "status_init" : "err",
                "msg"         : "Устройства не найдены",
            }
        return self.arduino_port_init
    
    
    
    def __init__(self, input_queque, output_queque) -> None:
        self.input_queque = input_queque
        self.output_queque = output_queque
        self.arduino_port_init = False
        
        thread = threading.Thread(target=self.tryInit, args=())
        thread.start()
        print(self.arduino_port_init)
        

    
    
    async def run(self):
    
    # self.try_init_port()
    # if self.sp != None:

        while True:
            # look for incoming tornado request
            if not self.input_queue.empty():
                client, msg = self.input_queue.get()
                resp = await self.request(msg)
                self.output_queque((client, resp))
                
    
    
    
    async def request(self, msg, type = "GET"):
        # resp = await asyncio.gather(self.__task())
        # self.sp.write("statinit, \n".encode(encoding = 'ascii'))
        resp = await asyncio.gather(self.__task(msg))

        return resp[0]
        # return  asyncio.create_task(self.__task())
        
        
            







# async def main():
#     cp   = ComPort()
#     # cl = [[i, print(i, await cp.request("statinit, "))] for i in range(4)]
    
#     resp = await cp.request("statinit, ")
#     print(resp)

    # print("resp :", 123)

    # resp1 = await asyncio.gather(cp.request("statinit, "))
    # print(resp, resp1)
    # resp = await cp.check_arduino() 
    # print("resp :", resp)
    # resp = await cp.check_arduino() 
    # print("resp :", )
    
    
    # queque_input = asyncio.Queue()
    # res1 = await asyncio.gather(cp.request("test1"))
    # res2 = await asyncio.gather(cp.request("test2"))
    # print(res1, res2)
    # print("go to ")
    
    
    # print(res)
    
    # look for incoming tornado request
        
        
    #     print(data.hex())
    #     print ("reading from serial: " + data.decode('utf-8'))
    #     send it back to tornado
        
        
        
# if __name__ == '__main__':
#     asyncio.run(main())