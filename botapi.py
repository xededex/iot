from aiogram import Bot, Dispatcher
from aiogram.filters import Command
from aiogram.types import Message
import os
import asyncio
import multiprocessing
import serialworker
import time
import threading


API_TOKEN: str = '6117883523:AAExjUbE-_fbGu9XFYN17b8SPKYzcGGptww'
bot: Bot = Bot(token=API_TOKEN)
dp: Dispatcher = Dispatcher()

input_queue = multiprocessing.Queue()
output_queue = multiprocessing.Queue()
cp = serialworker.ComPort(input_queue, output_queue)



async def checkQueue():
    if not output_queue.empty():
        client, message = output_queue.get()
        await client.answer(message)
    
    threading.Timer(1, checkQueue).start()








    
    

@dp.message(Command(commands=["reg"]))
async def search_all_controllers(message: Message):
    files = os.listdir("/dev/")
    contr = list(filter(lambda x : x.find("ttyUSB") != -1, files))
    print(contr)

    if len(contr) == 0:
        await message.answer('На сервере не найдено устройств')
        return
    await message.answer('Устройства :\n' + "\n".join(contr))
    


@dp.message(Command(commands=["found_ard"]))
async def process_start_command(message: Message):
    data = await cp.get_stat_arduino()
    await message.answer(str(data))


@dp.message(Command(commands=["smoke"]))
async def process_start_command(message: Message):
    data = cp.request("getlvlsmoke, ")
    await message.answer(str(data))


# Этот хэндлер будет срабатывать на команду "/start"
@dp.message(Command(commands=["start"]))
async def process_start_command(message: Message):
    data = cp.get_stat_arduino()
    await message.answer(str(data))

# Этот хэндлер будет срабатывать на команду "/help"
@dp.message(Command(commands=['help']))
async def process_help_command(message: Message):
    await message.answer('Напиши мне что-нибудь и в ответ '
                        'я пришлю тебе твое сообщение')


# Этот хэндлер будет срабатывать на любые ваши текстовые сообщения,
# кроме команд "/start" и "/help"
@dp.message()
async def send_echo(message: Message):
    resp = await cp.request(message.text)
    
    await message.reply(text = resp)


async def main():
    await dp.start_polling(bot)

if __name__ == "__main__":
    asyncio.run(main())
    
    
    # sp = serialworker.ComPort(input_queue, output_queue)
    # sp.daemon = True
    # sp.start()
    # checkQueue()
    # scheduler = PeriodicCallback(checkQueue, scheduler_interval)
	# scheduler.start()
    
    

    
    